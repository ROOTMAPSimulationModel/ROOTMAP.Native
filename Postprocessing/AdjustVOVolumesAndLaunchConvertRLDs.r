# AdjustVOVolumesAndLaunchConvertRLDs.r

# Modify within-VO volumes if necessary
require(tcltk)
VOVolumes <- tclVar(VOVolumeByDepth)
ttt<-tktoplevel()
entry.vovbd <-tkentry(ttt,width="100",textvariable=VOVolumes)
tkgrid(tklabel(ttt,text="Please enter a space-delimited list of volumes-within-VOs. Ensure this list is the same length as the list of depth points."))
tkgrid(entry.vovbd)
OnOK <- function()
{
	string <- strsplit(tclvalue(VOVolumes), split=" ")
	VOVolumeByDepth <<- as.vector(sapply(string, as.numeric))	# superassignment
	tkdestroy(ttt)
	
	source("Convert_Volume_RLDs_To_Arbitrary_Depth.r")
}
OK.but <-tkbutton(ttt,text="   OK   ",command=OnOK)
tkbind(entry.vovbd, "<Return>",OnOK)
tkgrid(OK.but)
tkfocus(ttt)