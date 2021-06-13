# GUISelectDataFiles.r
require(tcltk)
tt<-tktoplevel()
scr <- tkscrollbar(tt, repeatinterval=5, command=function(...)tkyview(tl,...))
tl<-tklistbox(tt,height=20, width=80, selectmode="multiple",yscrollcommand=function(...)tkset(scr,...),background="white")
tkgrid(tklabel(tt,text="Select target data file(s)"))
tkgrid(tl,scr,sticky="w")
tkgrid.configure(scr,rowspan=4,sticky="nsw")

if(exists("datasubdir"))
{
	basewd <- getwd()
	setwd(file.path(basewd,datasubdir))
}
filesInDirectory <- list.nondirfiles()

for (fn in filesInDirectory)
{
	tkinsert(tl,"end",fn)
}

OnOK <- function()
{
	# Take all selections (user will probably want to select multiple data files).
	selections <- as.numeric(tkcurselection(tl))+1
	tkdestroy(tt)

	# Superassignment
	filenames <<- filesInDirectory[selections]
	
	print("Selected data file(s): ")
	print(filenames)
	
	if(exists("datasubdir"))
	{
		setwd(basewd)
	}
	
	print("Configuration complete. Launching task selection dialog box.")
	source("GUISelectTask.r")
}
OK.but <-tkbutton(tt,text="   OK   ",command=OnOK)
tkgrid(OK.but)
tkfocus(tt)