# GUISelectTask.r

require(tcltk)
tt<-tktoplevel()
scr <- tkscrollbar(tt, repeatinterval=5, command=function(...)tkyview(tl,...))
tl<-tklistbox(tt,height=20, width=80, selectmode="single",yscrollcommand=function(...)tkset(scr,...),background="white")
tkgrid(tklabel(tt,text="Select script to execute"))
tkgrid(tl,scr,sticky="w")
tkgrid.configure(scr,rowspan=4,sticky="nsw")

filesInDirectory <- list.files(pattern="\\.[rR]$")	# Grab all files ending with .R or .r

doNotRun <- c("GUIShell.r","GUISelectConfigSubdir.r","GUISelectDataSubdir.r","GUISelectTask.r","GUISelectDataFile.r","Read_Boundaries.r","Calculate_Box_Volumes.r","Calculate_Box_Surface_Areas.r","AdjustVOVolumesAndLaunchConvertRLDs.r","DEBUG_Convert_Volume_RLDs_To_Arbitrary_Depth.r")

filesInDirectory <- setdiff(filesInDirectory,doNotRun)

for (fn in filesInDirectory)
{
	tkinsert(tl,"end",fn)
}

OnOK <- function()
{	
	# Take the first selection.
	selections <- as.numeric(tkcurselection(tl))+1
	tkdestroy(tt)
	
	print(paste("Launching task: ",filesInDirectory[selections[1]],sep=""))
	source(filesInDirectory[selections[1]])
}
OK.but <-tkbutton(tt,text="   OK   ",command=OnOK)
tkgrid(OK.but)
tkfocus(tt)