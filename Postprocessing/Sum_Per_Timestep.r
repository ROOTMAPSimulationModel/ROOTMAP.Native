# Sum_Per_Timestep.r

# Important note / TODO: all R scripts expect XZ plane slices. Gotta update 'em to read the OutputRules' <specification1>
if(exists("basewd"))	rm(basewd)
if(exists("separator"))	rm(separator)
if(exists("filesInDirectory"))	rm(filesInDirectory)
if(exists("cb"))	rm(cb)
if(exists("cbValue"))	rm(cbValue)
if(exists("scr"))	rm(scr)
if(exists("filenames"))	rm(filenames)
if(exists("tl"))	rm(tl)
if(exists("fn"))	rm(fn)
if(exists("tt"))	rm(tt)
if(exists("OK.but"))	rm(OK.but)
if(exists("OnOK"))	rm(OnOK)

basewd <- getwd()
if(exists("configsubdir"))
{
	setwd(file.path(basewd,configsubdir))
}

separator <- "," # TODO read this from wrapper script

# Tcl/Tk list box code adapted from http://bioinf.wehi.edu.au/~wettenhall/RTclTkExamples/listboxes.html
	
require(tcltk)
tt<-tktoplevel()

scr <- tkscrollbar(tt, repeatinterval=5, command=function(...)tkyview(tl,...))
tl<-tklistbox(tt,height=20, width=80, selectmode="multiple",yscrollcommand=function(...)tkset(scr,...),background="white")
tkgrid(tklabel(tt,text="Select files you wish to sum per-timestep"))
tkgrid(tl,scr,sticky="w")
tkgrid.configure(scr,rowspan=4,sticky="nsw")

if(exists("datasubdir"))
{
	setwd(file.path(basewd,datasubdir))
}
filesInDirectory <- list.files()

for (fn in filesInDirectory)
{
	tkinsert(tl,"end",fn)
}
cb <- tkcheckbutton(tt)
cbValue <- tclVar("0")
tkconfigure(cb,variable=cbValue)
tkgrid(tklabel(tt,text="Add a column summing all selected files per-Timestep?"),cb)

OnOK <- function()
{
	if(as.character(tclvalue(cbValue)) == 1)
	{
		sumAllFiles <- TRUE
	}
	else
	{
		sumAllFiles <- FALSE
	}
	
	selections <- as.numeric(tkcurselection(tl))+1
	tkdestroy(tt)
	
	selectedFiles <- array(length(selections))
	for(i in 1:length(selections))
	{
		selectedFiles[i] <- filesInDirectory[selections[i]]
		i <- i+1
	}
	# Use superassignment to make values available to the global environment.
	filenames <<- selectedFiles

	setwd(basewd)	

	source(file.path(basewd,"Read_Boundaries.r"))

	# GAH! Ugly
	maxTimesteps <- 0	
	if(exists("datasubdir"))
	{
		setwd(file.path(basewd,datasubdir))
	}
	for(fn in filenames)
	{
		# Use superassignment to make values available to the global environment.
		filename <<- file.path(getwd(),fn)
		source(file.path(basewd,"Read_Raw_Data.r"))
		
		maxTimesteps <- max(maxTimesteps, numTimesteps)
	}

	firstFile <- TRUE

	for(fn in filenames)
	{
		# Use superassignment to make values available to the global environment.
		filename <<- file.path(getwd(),fn)
		source(file.path(basewd,"Read_Raw_Data.r"))
		
		maxTimesteps <- max(maxTimesteps, numTimesteps)

		# Array of values per timestep
		sumsPerTimestep <- array(dim=numTimesteps)

		for(iT in 1:numTimesteps)
		{
			sumsPerTimestep[iT] <- 0.0
			for(iSPT in 1:numSlicesPerTimestep)
			{
				start <- 1 + (iT-1)*numSlicesPerTimestep*length(zb) + (iSPT-1)*length(zb)
				end <- start + length(zb)-2
				sumsPerTimestep[iT] <- sumsPerTimestep[iT] + sum(rawData[c(start:end),]) # add the sum of this "slice"
			}
		}
		
		while(length(sumsPerTimestep)<maxTimesteps)
		{
			sumsPerTimestep <- c(sumsPerTimestep, NA) 
		}
		
		if(firstFile==TRUE)
		{
			results <- sumsPerTimestep
			firstFile <- FALSE
		}
		else
		{
			results <- cbind(results, sumsPerTimestep)
		}
	}

	columns <- c("Timestep",filenames)

	if(sumAllFiles)
	{
		sumOfAll <- rowSums(results, na.rm=TRUE)	# Omit all NA values (i.e. treat as 0) when summing
		results <- cbind(results, sumOfAll)
		columns <- c(columns,"Sum of all files")
	}

	namePrefix <- "SumPerTimestep"
	filenameOK <- FALSE
	serialNumber <- 0
	while(!filenameOK)
	{
		nameString <- paste(sep="", namePrefix, serialNumber)
		path <- file.path(getwd(),paste(sep="", nameString, ".csv"))
		serialNumber <- serialNumber+1
		if(!file.exists(path))
		{
			filenameOK <- TRUE
			outputFile <- file(path)
		}
	}
	timeStep <- c(1:maxTimesteps)
	write.table(sep=",", col.names=columns, data.frame(timeStep,results), file=outputFile, row.names=FALSE)

	setwd(basewd)
}
OK.but <-tkbutton(tt,text="   OK   ",command=OnOK)
tkgrid(OK.but)
tkfocus(tt)