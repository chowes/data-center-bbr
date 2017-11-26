#!/usr/bin/Rscript

setwd("~/Documents/school/data-center-bbr/tests/results/")

library(ggplot2)
library(plotrix)
library(Hmisc)

source("plots.R")

query_bbr <- read.csv(file = "query/n_100/query_bbr.csv", header = TRUE)
query_cubic <- read.csv(file = "query/n_100/query_cubic.csv", header = TRUE)
query_bbr_bg <- read.csv(file = "query/n_100/query_bbr_bg.csv", header = TRUE)
query_cubic_bg <- read.csv(file = "query/n_100/query_cubic_bg.csv", header = TRUE)
query_bbr$cong_ctl <- "bbr" 
query_bbr_bg$cong_ctl <- "bbr" 
query_cubic$cong_ctl <- "cubic"
query_cubic_bg$cong_ctl <- "cubic"

query_bbr$bg <- "false" 
query_bbr_bg$bg <- "true" 
query_cubic$bg <- "false"
query_cubic_bg$bg <- "true"

query_data <- rbind(query_bbr, query_cubic, query_bbr_bg, query_cubic_bg)


query_delay_avg(subset(query_data, bg == "false"), "test", 80, F)
query_delay_percentile(subset(query_data, bg == "false"), "test", 12, F)
query_cdf_graph(subset(subset(query_data, bg == "false"), num_workers == 20), "test", 12, F)

query_delay_avg(subset(query_data, bg == "true"), "test", 400, F)
query_delay_percentile(subset(query_data, bg == "true"), "test", 800, F)
query_cdf_graph(subset(subset(query_data, bg == "true"), num_workers == 20), "test", 400, F)

# generate convergance graph
converg_graph(subset(converg_data, cong_ctl == "dctcp"), "paper/figures/dctcp_converg.pdf", save=FALSE)
converg_graph(subset(converg_data, cong_ctl == "reno"), "paper/figures/reno_converg.pdf", save=FALSE)

# generate throughput graph based on K, no effect after 20 so we can cut it off at k = 60
k_throughput_graph(subset(throughput_data, k <= 30), "paper/figures/k_throughput_delay.pdf", save=TRUE)