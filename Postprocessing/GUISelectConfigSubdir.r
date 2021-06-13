# GUISelectDataSubdir.r
require(tcltk)
tt<-tktoplevel()
scr <- tkscrollbar(tt, repeatinterval=5, command=function(...)tkyview(tl,...))
tl<-tklistbox(tt,height=20, width=80, selectmode="single",yscrollcommand=function(...)tkset(scr,...),background="white")
tkgrid(tklabel(tt,text="Select subdirectory containing config files, or just click OK to use this directory."))
tkgrid(tl,scr,sticky="w")
tkgrid.configure(scr,rowspan=4,sticky="nsw")

directories <- list.dirs()


for (fn in directories)
{
	tkinsert(tl,"end",fn)
}

OnOK <- function()
{
	# Take the first selection.
	selections <- as.numeric(tkcurselection(tl))+1
	tkdestroy(tt)
	
	# Superassignment
	configsubdir <<- directories[selections[1]]
	
	if(is.na(configsubdir)) 
	{
		# Again, superassignment
		configsubdir <<- ""
		print("Selected configuration directory: The current working directory")
	}
	else
	{	
		print(paste("Selected configuration subdirectory: ",configsubdir,".",sep=""))
	}

	source("Read_Boundaries.r")
	source("Calculate_Box_Volumes.r")
	source("Calculate_Box_Surface_Areas.r")
	source("GUISelectDataSubdir.r")
}
OK.but <-tkbutton(tt,text="   OK   ",command=OnOK)
tkgrid(OK.but)
tkfocus(tt)