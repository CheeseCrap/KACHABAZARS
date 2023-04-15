
#!/usr/bin/Rscript
filename <- "wikipedia_data.png"
data <- read.table("data/wikipedia.data", header=FALSE, sep="," ,comment.char="#")
png(filename)
plot(data$V1[1:156],data$V2[1:156],col='red',xlab="X",ylab="Y",main="Original Data")