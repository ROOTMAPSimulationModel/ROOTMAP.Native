# GUIConvertVolumeRLDsToArbitraryDepth.r

# Initialise default values for:
# 1) Arbitrary depths vector
arbitraryDepths <- tclVar(zb[2:length(zb)])
# 2) Aggregate within-VO volumes by depth
VOVolumeByDepth <- c()
for(i in 1:(length(zb)-1))
{
	VOVolumeByDepth <- c(VOVolumeByDepth,sum(boxVolumes[i,,]))
}

require(tcltk)
tt<-tktoplevel()
entry.arbitraryDepths <-tkentry(tt,width="100",textvariable=arbitraryDepths)
tkgrid(tklabel(tt,text="Please enter a space-delimited list of depth points, in descending order and without any non-numeric characters."))
tkgrid(entry.arbitraryDepths)
OnOK <- function()
{
	string <- strsplit(tclvalue(arbitraryDepths), split=" ")
	arbitraryDepths <<- as.vector(sapply(string, as.numeric))	# superassignment
	tkdestroy(tt)
	source("AdjustVOVolumesAndLaunchConvertRLDs.r")
}
OK.but <-tkbutton(tt,text="   OK   ",command=OnOK)
tkbind(entry.arbitraryDepths, "<Return>",OnOK)
tkgrid(OK.but)
tkfocus(tt)