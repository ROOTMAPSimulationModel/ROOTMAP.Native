# Sum_Per_Plant.r

# Important note: all R scripts expect XZ plane slices. Gotta update 'em to read the OutputRules' <specification1>

rm(list=ls())

separator <- ","

# Tcl/Tk list box code adapted from http://bioinf.wehi.edu.au/~wettenhall/RTclTkExamples/listboxes.html

require(tcltk)
tt<-tktoplevel()

scr <- tkscrollbar(tt, repeatinterval=5, command=function(...)tkyview(tl,...))
tl<-tklistbox(tt,height=20, width=80, selectmode="multiple",yscrollcommand=function(...)tkset(scr,...),background="white")
tkgrid(tklabel(tt,text="Select files you wish to sum per-timestep"))
tkgrid(tl,scr,sticky="w")
tkgrid.configure(scr,rowspan=4,sticky="nsw")
filesInDir <- list.files()
for (fn in filesInDir)
{
    tkinsert(tl,"end",fn)
}
cb <- tkcheckbutton(tt)
cbValue <- tclVar("0")
tkconfigure(cb,variable=cbValue)
tkgrid(tklabel(tt,text="Sum all values per-Timestep?"),cb)

OnOK <- function()
{
    sumAllFiles <- as.character(tclvalue(cbValue))
	print("Summing all files?")
	print(sumAllFiles)
	
	selections <- as.numeric(tkcurselection(tl))+1
	files <- array(length(selections))
	for(i in 1:length(selections))
	{
		files[i] <- filesInDir[selections[i]]
		i <- i+1
	}
    tkdestroy(tt)
	
	# Data processing code below.
	
	filenames <- files
	
	source(file.path("C:/RM/build/win32_d","Read_Boundaries.r"))

	# GAH! Ugly
	maxTimesteps <- 0
	for(filename in filenames)
	{
		filename <- file.path("C:/RM/build/win32_d",filename)
		source(file.path("C:/RM/build/win32_d","Read_Raw_Data.r"))
		
		maxTimesteps <- max(maxTimesteps, numTimesteps)
	}

	firstFile <- TRUE

	for(filename in filenames)
	{
		filename <- file.path("C:/RM/build/win32_d",filename)
		source(file.path("C:/RM/build/win32_d","Read_Raw_Data.r"))
		
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

	# Total Aggregate Root Lengths per timestep
	outputFile <- file(paste(sep="", "C:/RM/build/win32_d/", "TEMP", ".csv"))
	timeStep <- c(1:maxTimesteps)
	write.table(sep=",", col.names=columns, data.frame(timeStep,results), file=outputFile, row.names=FALSE)

		
}
OK.but <-tkbutton(tt,text="   OK   ",command=OnOK)
tkgrid(OK.but)
tkfocus(tt)

