# GUISelectDataSubdir.r
require(tcltk)
tt<-tktoplevel()
scr <- tkscrollbar(tt, repeatinterval=5, command=function(...)tkyview(tl,...))
tl<-tklistbox(tt,height=20, width=80, selectmode="single",yscrollcommand=function(...)tkset(scr,...),background="white")
tkgrid(tklabel(tt,text="Select subdirectory containing data files, or just click OK to use this directory."))
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
	datasubdir <<- directories[selections[1]]
	
	if(is.na(datasubdir)) 
	{
		# Again, superassignment
		datasubdir <<- ""
		print("Selected data directory: The current working directory")
	}
	else
	{	
		print(paste("Selected data subdirectory: ",datasubdir,".",sep=""))
	}
	
	source("GUISelectDataFiles.r")
}
OK.but <-tkbutton(tt,text="   OK   ",command=OnOK)
tkgrid(OK.but)
tkfocus(tt)