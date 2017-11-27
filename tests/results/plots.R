#!/usr/bin/Rscript

require(ggplot2)
require(Hmisc)

percentile <- function(data) {
  result = quantile(data, 0.95)
  
  return(result)
}


query_delay_avg <- function(data, filename, xlim, ylim, save=FALSE) {
  
  plot <- ggplot(data, aes(x=num_workers, y=response_time/1000, color=cong_ctl)) +
    stat_summary(fun.y=mean, geom="line", size=1.5) +
    stat_summary(geom="errorbar", fun.data=mean_cl_normal) +
    labs(x = "Number of Workers", y = "Response Time (msecs)") +
    coord_cartesian(ylim = c(0, ylim), xlim=c(1, xlim)) +
    scale_y_continuous(expand = c(0, 0)) +
    scale_x_continuous(breaks = seq(0, xlim, by = 5)) +
    # scale_linetype_manual(limits = c("cubic", "bbr"), breaks = c("cubic", "bbr"), values = c("dashed", "twodash"), guide=F) + 
    scale_color_manual(limits = c("cubic", "bbr"), breaks = c("cubic", "bbr"), values = c("red", "blue"), labels = c("TCP Cubic", "TCP BBR")) +
  
    theme_bw() + 
    theme(axis.title.y = element_text(size=20, margin = margin(0, 15, 0, 0), face="bold"),
          axis.title.x = element_text(size=20, margin = margin(15, 0, 0, 0), face="bold"),
          axis.text.y = element_text(size=16, color = "black"), 
          axis.text.x = element_text(size=20, color = "black"), 
          axis.ticks = element_line(size=1.6, color = "black"),
          legend.text = element_text(size = 20, colour = "black", face = "bold"),
          legend.title = element_blank(),
          legend.key = element_blank(),
          legend.key.size = unit(1, "cm"),
          panel.grid.major = element_blank(),
          panel.grid.minor = element_blank(),
          panel.border = element_blank(),
          panel.background = element_blank(),
          axis.line.x = element_line(colour = 'black', size = 1.6),
          axis.line.y = element_line(colour = 'black', size = 1.6),
          plot.margin=unit(c(.5,.5,.5,.5), "cm"))
  
  if (save) {
    ggsave(plot=plot, filename=filename, device="pdf", width=10, height=5)
  } else {
    print(plot)
  }
}


query_delay_percentile <- function(data, filename, xlim, ylim, save=FALSE) {
  
  plot <- ggplot(data, aes(x=num_workers, y=response_time/1000, color=cong_ctl)) +
    stat_summary(fun.y=percentile, geom="line", size=1.5) +
    labs(x = "Number of Workers", y = "Response Time (msecs)") +
    coord_cartesian(ylim = c(0, ylim), xlim=c(1, xlim)) +
    scale_y_continuous(expand = c(0, 0)) +
    scale_x_continuous(breaks = seq(0, xlim, by = 5)) +
    # scale_linetype_manual(limits = c("cubic", "bbr"), breaks = c("cubic", "bbr"), values = c("dashed", "twodash"), guide=F) + 
    scale_color_manual(limits = c("cubic", "bbr"), breaks = c("cubic", "bbr"), values = c("red", "blue"), labels = c("TCP Cubic", "TCP BBR")) +
    
    theme_bw() + 
    theme(axis.title.y = element_text(size=20, margin = margin(0, 15, 0, 0), face="bold"),
          axis.title.x = element_text(size=20, margin = margin(15, 0, 0, 0), face="bold"),
          axis.text.y = element_text(size=16, color = "black"), 
          axis.text.x = element_text(size=20, color = "black"), 
          axis.ticks = element_line(size=1.6, color = "black"),
          legend.text = element_text(size = 20, colour = "black", face = "bold"),
          legend.title = element_blank(),
          legend.key = element_blank(),
          legend.key.size = unit(1, "cm"),
          panel.grid.major = element_blank(),
          panel.grid.minor = element_blank(),
          panel.border = element_blank(),
          panel.background = element_blank(),
          axis.line.x = element_line(colour = 'black', size = 1.6),
          axis.line.y = element_line(colour = 'black', size = 1.6),
          plot.margin=unit(c(.5,.5,.5,.5), "cm"))
  
  if (save) {
    ggsave(plot=plot, filename=filename, device="pdf", width=10, height=5)
  } else {
    print(plot)
  }
}


query_cdf_graph <- function(data, filename, xlim, save=FALSE) {
  
  plot <- ggplot(data, aes(response_time/1000, color=cong_ctl)) +
    stat_ecdf(size=1.5) +
    labs(x = "Repsonse Time (msecs)", y = "Cumulative Fraction") +
    coord_cartesian(ylim = c(0, 1), xlim=c(0, xlim)) +
    scale_y_continuous(expand = c(0, 0)) +
    scale_color_manual(limits = c("cubic", "bbr"), breaks = c("cubic", "bbr"), values = c("red", "blue"), labels = c("TCP Cubic", "TCP BBR")) + 
    # scale_linetype_manual(limits = c("cubic", "bbr"), breaks = c("cubic", "bbr"), values = c("dashed", "twodash"), guide=F) + 
    theme_bw() + 
    theme(axis.title.y = element_text(size=20, margin = margin(0, 15, 0, 0), face="bold"),
          axis.title.x = element_text(size=20, margin = margin(15, 0, 0, 0), face="bold"),
          axis.text.y = element_text(size=16, color = "black"), 
          axis.text.x = element_text(size=20, color = "black"), 
          axis.ticks = element_line(size=1.6, color = "black"),
          legend.text = element_text(size = 20, colour = "black", face = "bold"),
          legend.title = element_blank(),
          legend.key = element_blank(),
          legend.key.size = unit(1, "cm"),
          panel.grid.major = element_blank(),
          panel.grid.minor = element_blank(),
          panel.border = element_blank(),
          panel.background = element_blank(),
          axis.line.x = element_line(colour = 'black', size = 1.6),
          axis.line.y = element_line(colour = 'black', size = 1.6),
          plot.margin=unit(c(.5,.5,.5,.5), "cm"))
  
  if (save) {
    ggsave(plot=plot, filename=filename, device="pdf", width=10, height=5)
  } else {
    print(plot)
  }
}


thru_avg <- function(data, filename, xlim, ylim, save=FALSE) {
  
  plot <- ggplot(data, aes(x=num_flows, y=throughput, color=cong_ctl)) +
    stat_summary(fun.y=mean, geom="line", size=1.5) +
    stat_summary(geom="errorbar", fun.data=mean_cl_normal) +
    labs(x = "Number of Flows", y = "Throughput (Mbits/second)") +
    coord_cartesian(ylim = c(0, ylim), xlim=c(1, xlim)) +
    scale_y_continuous(expand = c(0, 0)) +
    scale_x_continuous(breaks = seq(0, xlim, by = 5)) +
    # scale_linetype_manual(limits = c("cubic", "bbr"), breaks = c("cubic", "bbr"), values = c("dashed", "twodash"), guide=F) + 
    scale_color_manual(limits = c("cubic", "bbr"), breaks = c("cubic", "bbr"), values = c("red", "blue"), labels = c("TCP Cubic", "TCP BBR")) +
    
    theme_bw() + 
    theme(axis.title.y = element_text(size=20, margin = margin(0, 15, 0, 0), face="bold"),
          axis.title.x = element_text(size=20, margin = margin(15, 0, 0, 0), face="bold"),
          axis.text.y = element_text(size=16, color = "black"), 
          axis.text.x = element_text(size=20, color = "black"), 
          axis.ticks = element_line(size=1.6, color = "black"),
          legend.text = element_text(size = 20, colour = "black", face = "bold"),
          legend.title = element_blank(),
          legend.key = element_blank(),
          legend.key.size = unit(1, "cm"),
          panel.grid.major = element_blank(),
          panel.grid.minor = element_blank(),
          panel.border = element_blank(),
          panel.background = element_blank(),
          axis.line.x = element_line(colour = 'black', size = 1.6),
          axis.line.y = element_line(colour = 'black', size = 1.6),
          plot.margin=unit(c(.5,.5,.5,.5), "cm"))
  
  if (save) {
    ggsave(plot=plot, filename=filename, device="pdf", width=10, height=5)
  } else {
    print(plot)
  }
}


thru_percentile <- function(data, filename, xlim, ylim, save=FALSE) {
  
  plot <- ggplot(data, aes(x=num_flows, y=throughput, color=cong_ctl)) +
    stat_summary(fun.y=percentile, geom="line", size=1.5) +
    labs(x = "Number of Flows", y = "Throughput (Mbits/second)") +
    coord_cartesian(ylim = c(0, ylim), xlim=c(1, xlim)) +
    scale_y_continuous(expand = c(0, 0)) +
    scale_x_continuous(breaks = seq(0, xlim, by = 5)) +
    # scale_linetype_manual(limits = c("cubic", "bbr"), breaks = c("cubic", "bbr"), values = c("dashed", "twodash"), guide=F) + 
    scale_color_manual(limits = c("cubic", "bbr"), breaks = c("cubic", "bbr"), values = c("red", "blue"), labels = c("TCP Cubic", "TCP BBR")) +
    
    theme_bw() + 
    theme(axis.title.y = element_text(size=20, margin = margin(0, 15, 0, 0), face="bold"),
          axis.title.x = element_text(size=20, margin = margin(15, 0, 0, 0), face="bold"),
          axis.text.y = element_text(size=16, color = "black"), 
          axis.text.x = element_text(size=20, color = "black"), 
          axis.ticks = element_line(size=1.6, color = "black"),
          legend.text = element_text(size = 20, colour = "black", face = "bold"),
          legend.title = element_blank(),
          legend.key = element_blank(),
          legend.key.size = unit(1, "cm"),
          panel.grid.major = element_blank(),
          panel.grid.minor = element_blank(),
          panel.border = element_blank(),
          panel.background = element_blank(),
          axis.line.x = element_line(colour = 'black', size = 1.6),
          axis.line.y = element_line(colour = 'black', size = 1.6),
          plot.margin=unit(c(.5,.5,.5,.5), "cm"))
  
  if (save) {
    ggsave(plot=plot, filename=filename, device="pdf", width=10, height=5)
  } else {
    print(plot)
  }
}


thru_cdf_graph <- function(data, filename, xlim, save=FALSE) {
  
  plot <- ggplot(data, aes(throughput, color=cong_ctl)) +
    stat_ecdf(size=1.5) +
    labs(x = "Throughput (Mbits/second)", y = "Cumulative Fraction") +
    coord_cartesian(ylim = c(0, 1), xlim=c(0, xlim)) +
    scale_y_continuous(expand = c(0, 0)) +
    scale_color_manual(limits = c("cubic", "bbr"), breaks = c("cubic", "bbr"), values = c("red", "blue"), labels = c("TCP Cubic", "TCP BBR")) + 
    # scale_linetype_manual(limits = c("cubic", "bbr"), breaks = c("cubic", "bbr"), values = c("dashed", "twodash"), guide=F) + 
    theme_bw() + 
    theme(axis.title.y = element_text(size=20, margin = margin(0, 15, 0, 0), face="bold"),
          axis.title.x = element_text(size=20, margin = margin(15, 0, 0, 0), face="bold"),
          axis.text.y = element_text(size=16, color = "black"), 
          axis.text.x = element_text(size=20, color = "black"), 
          axis.ticks = element_line(size=1.6, color = "black"),
          legend.text = element_text(size = 20, colour = "black", face = "bold"),
          legend.title = element_blank(),
          legend.key = element_blank(),
          legend.key.size = unit(1, "cm"),
          panel.grid.major = element_blank(),
          panel.grid.minor = element_blank(),
          panel.border = element_blank(),
          panel.background = element_blank(),
          axis.line.x = element_line(colour = 'black', size = 1.6),
          axis.line.y = element_line(colour = 'black', size = 1.6),
          plot.margin=unit(c(.5,.5,.5,.5), "cm"))
  
  if (save) {
    ggsave(plot=plot, filename=filename, device="pdf", width=10, height=5)
  } else {
    print(plot)
  }
}


converg_graph <- function(converg_data, filename, save=FALSE) {
  
  plot <- ggplot(converg_data, aes(x=time/1000000, y=throughput, color=socket_fd)) +
    geom_line(size=1) +
    labs(x = "Time (Seconds)", y = "Throughput (Mbits/Second)") +
    coord_cartesian(ylim = c(0, 1000), xlim=c(0, 270)) +
    scale_y_continuous(expand = c(0, 0)) +
    scale_x_continuous(breaks = seq(0, 270, by = 30)) +
    # scale_color_manual(limits = c("h2", "h3", "h4", "h5", "h6"), breaks = c("h2", "h3", "h4", "h5", "h6"), values = c("purple", "red", "green", "blue", "black"), labels = c("Flow 1", "Flow 2", "Flow 3", "Flow 4", "Flow 5")) +
    theme_bw() + 
    theme(axis.title.y = element_text(size=20, margin = margin(0, 15, 0, 0), face="bold"),
          axis.title.x = element_text(size=20, margin = margin(15, 0, 0, 0), face="bold"),
          axis.text.y = element_text(size=16, color = "black"), 
          axis.text.x = element_text(size=20, color = "black"), 
          axis.ticks = element_line(size=1.6, color = "black"),
          legend.text = element_text(size = 20, colour = "black", face = "bold"),
          legend.title = element_blank(),
          legend.key = element_blank(),
          legend.key.size = unit(1, "cm"),
          panel.grid.major = element_blank(),
          panel.grid.minor = element_blank(),
          panel.border = element_blank(),
          panel.background = element_blank(),
          axis.line.x = element_line(colour = 'black', size = 1.6),
          axis.line.y = element_line(colour = 'black', size = 1.6),
          plot.margin=unit(c(.5,.5,.5,.5), "cm"))
  
  if (save) {
    ggsave(plot=plot, filename=filename, width=10, height=5)
  } else {
    print(plot)
  }
}


k_throughput_graph <- function(throughput_data, filename, save=FALSE) {
  plot <- ggplot(throughput_data, aes(x=k, y=thru)) +
    geom_line(size = 1.2) +
    labs(x = "K", y = "Throughput (Mbps)") +
    coord_cartesian(ylim = c(0, 100), xlim=c(0, 30)) +
    scale_y_continuous(expand = c(0, 0)) +
    scale_x_continuous(breaks = seq(0, 30, by = 10)) +
    theme_bw() + 
    theme(axis.title.y = element_text(size=20, margin = margin(0, 15, 0, 0), face="bold"),
          axis.title.x = element_text(size=20, margin = margin(15, 0, 0, 0), face="bold"),
          axis.text.y = element_text(size=16, color = "black"), 
          axis.text.x = element_text(size=20, color = "black"), 
          axis.ticks = element_line(size=1.6, color = "black"),
          panel.grid.major = element_blank(),
          panel.grid.minor = element_blank(),
          panel.border = element_blank(),
          panel.background = element_blank(),
          axis.line.x = element_line(colour = 'black', size = 1.6),
          axis.line.y = element_line(colour = 'black', size = 1.6),
          plot.margin=unit(c(.5,.5,.5,.5), "cm"))
  
  if (save) {
    ggsave(plot=plot, filename=filename, width=8, height=5)
  } else {
    print(plot)
  }
}