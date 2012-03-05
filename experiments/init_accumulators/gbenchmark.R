
gbench <- function(file_name) {

	input <- read.table(file_name, header=TRUE, comment.char="#", sep=",")

	postings <- unique(input$Postings.List.Length)

	num_of_plots <- length(postings)
	par(mfrow = c(num_of_plots/2, 2), mar = c(4,5,4,5))


	for (p in postings) {
		subinput <- subset(input, Postings.List.Length == p)
	
		low_y = min(subinput$Array.Time, subinput$Tree.Time)
		high_y = max(subinput$Array.Time, subinput$Tree.Time)
	
		x_label = sprintf("width in bits (postings length = %d)", p)
	
		plot(subinput$Array.Time, type="o", ylim=c(low_y, high_y), col="blue", 
				xlab=x_label, ylab="ms")
		lines(subinput$Tree.Time, type="o", ylim=c(low_y, high_y), col="red")
		#axis(1, at=1:20, lab=subinput$Width.in.Bits)
	}
}

