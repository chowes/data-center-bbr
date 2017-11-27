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

thru_bbr <- read.csv(file = "throughput/thru_bbr.csv", header = TRUE)
thru_cubic <- read.csv(file = "throughput/thru_cubic.csv", header = TRUE)
thru_bbr$cong_ctl <- "bbr" 
thru_cubic$cong_ctl <- "cubic"

thru_data <- rbind(thru_bbr, thru_cubic)

converge_bbr <- read.csv(file = "converge/bbr_converge.csv", header = TRUE)
converge_cubic <- read.csv(file = "converge/cubic_converge.csv", header = TRUE)
converge_bbr$cong_ctl <- "bbr" 
converge_cubic$cong_ctl <- "cubic"

converge_data <- rbind(converge_bbr, converge_cubic)
converge_data$socket_fd <- as.factor(converge_data$socket_fd)


query_delay_avg(subset(query_data, bg == "false"), "query_avg.pdf", 40, 15, T)
query_delay_percentile(subset(query_data, bg == "false"), "query_percentile.pdf", 40, 15, T)
query_cdf_graph(subset(subset(query_data, bg == "false"), num_workers == 20), "query_cdf.pdf", 15, T)

query_delay_avg(subset(query_data, bg == "true"), "query_bg_avg.pdf", 20, 800, T)
query_delay_percentile(subset(query_data, bg == "true"), "query_bg_percentile.pdf", 20, 800, T)
query_cdf_graph(subset(subset(query_data, bg == "true"), num_workers == 20), "query_bg_cdf.pdf", 300, T)

thru_avg(subset(thru_data, socket_fd == "all" & time == "total"), "thru_avg.pdf", 20, 1000, T)
thru_percentile(subset(thru_data, socket_fd == "all" & time == "total"), "thru_percentile.pdf", 20, 1000, T)
thru_cdf_graph(subset(thru_data, socket_fd == "all" & time == "total" & num_flows == 20), "thru_cdf.pdf", 1000, T)



# generate convergance graph
converg_graph(subset(converge_data, cong_ctl == "bbr"), "paper/figures/dctcp_converg.pdf", save=FALSE)
converg_graph(subset(converge_data, cong_ctl == "cubic"), "paper/figures/reno_converg.pdf", save=FALSE)

# generate throughput graph based on K, no effect after 20 so we can cut it off at k = 60
k_throughput_graph(subset(throughput_data, k <= 30), "paper/figures/k_throughput_delay.pdf", save=TRUE)