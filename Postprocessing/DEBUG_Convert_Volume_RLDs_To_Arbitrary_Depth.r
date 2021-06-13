# Debug version of this script, to demonstrate functionality.
# Ensure you have run Average_RLD.r on a set of lupin genotype data before running this script.
# Uncomment your desired "arbitrary depths" vector & VO volume vector here, then run the whole script
# (with focus in this R Editor window, select all text and run. Windows shortcut: Ctrl-A, Ctrl-R.)


# arbitraryDepths <- zb[2:length(zb)]	# Set arbitraryDepths to the same as the actual data depths. Thus volumeRLDsByArbitraryDepth should equal volumeRLDs.
# VOVolumeByDepth <- c(rep(78.5,times=3), rep(39.3,times=6), rep(78.5,times=53)) # Volumes from the Excel file.

#  arbitraryDepths <- c(1:5, 2*3:30)		# These are the depths from the Excel file.
#  VOVolumeByDepth <- c(rep(78.5,times=6), rep(2*78.5,times=27)) # Derived from volumes in the Excel file.

# arbitraryDepths <- c(0.35,12,13,14.55,14.56,20,47,48,60,64)	# Some crazy values, including one which goes beyond the actual data (actual data max. 60)
# VOVolumeByDepth <- c(123,344,666,7654,97,123,344,666,7654,97)	# Nonsense values.


print("Using arbitraryDepths vector:")
print(arbitraryDepths)
print("And VOVolumeByDepth vector:")
print(VOVolumeByDepth)

volumeRLDsByArbitraryDepth <- array( ,c(length(arbitraryDepths),numCols,numSlices))

Scoreboard_TRL <- seq(0,0,length=length(arbitraryDepths))
Scoreboard_TRLD <- seq(0,0,length=length(arbitraryDepths))
VO_TRLD <- seq(0,0,length=length(arbitraryDepths))

layertop <- 0.0

for(i in 1:length(arbitraryDepths))
{
	if(i>1) 
	{ 
		layertop <- arbitraryDepths[i-1]
	}
	layerbottom <- arbitraryDepths[i]
	sblayertop <- 0.0

	thisSliceVolume <- 0.0

	print(paste("ArbitraryDepth layer ",i,": ",layertop," to ",layerbottom," cm deep"))

	thisSection <- array(,c(numCols,numSlices))
	# Zero everything out, just in case we don't find any good data. Also to facilitate += operations
	for(a in 1:numCols)
	{
		for(b in 1:numSlices)
		{
			thisSection[a,b] <- 0.0
		}
	}

	for(j in 1:numRows)
	{
		sblayertop <- zb[j]
		sblayerbottom <- zb[j+1]

		SBTAboveDT <- TRUE
		SBTAtDT <- FALSE
		SBTAboveDB <- TRUE
		SBBBelowDT <- FALSE
		SBBAtDB <- FALSE
		SBBBelowDB <- FALSE
		
		if(sblayertop==layertop)
		{
			SBTAtDT <- TRUE
		} else if(sblayertop>layertop)
		{
			SBTAboveDT <- FALSE
		}
		if(sblayertop>layerbottom)
		{
			SBTAboveDB <- FALSE
		}
		if(sblayerbottom==layerbottom)
		{
			SBBAtDB <- TRUE
		} else if(sblayerbottom>layertop)
		{
			SBBBelowDT <- TRUE
		}
		if(sblayerbottom>layerbottom)
		{
			SBBBelowDB <- TRUE
		}

		if(!SBTAboveDB && SBBBelowDB)
		{
			# Bail out if this Scoreboard layer is entirely deeper than the Depth layer.
			break
		}

		if(SBTAtDT && SBBAtDB)
		{
			# Layers are equal. Just use the existing data.
			print(paste("Scoreboard layer ",j,": ",sblayertop," to ",sblayerbottom," cm deep"))
			print("Using this existing layer of data as is")
			thisSection <- volumeRLDs[j,,]
			thisSliceVolume <- sum(boxVolumes[j,,])
			break
		} else if(SBTAboveDT && SBBBelowDB)
		{
			# This Scoreboard layer overlaps this Depth layer at both top and bottom.
			proportion <- (layerbottom - layertop) / (sblayerbottom - sblayertop)
			print(paste("Scoreboard layer ",j,": ",sblayertop," to ",sblayerbottom," cm deep"))
			print(paste("Using ",proportion," of this existing layer of data only"))
			thisSection <- proportion*volumeRLDs[j,,]
			thisSliceVolume <- proportion*sum(boxVolumes[j,,])
			break
		} else if(!SBTAboveDT && SBBBelowDT && !SBBBelowDB)
		{
			# This Scoreboard layer falls entirely within the Depth layer.			
			print(paste("Scoreboard layer ",j,": ",sblayertop," to ",sblayerbottom," cm deep"))
			print("Using all of this existing layer of data (and portions of others?)")
			thisSection <- thisSection + volumeRLDs[j,,]
			thisSliceVolume <- thisSliceVolume + sum(boxVolumes[j,,])
		} else if(SBTAboveDT && SBBBelowDT && !SBBBelowDB)
		{
			proportion <- (sblayerbottom - layertop) / (sblayerbottom - sblayertop)
			print(paste("Scoreboard layer ",j,": ",sblayertop," to ",sblayerbottom," cm deep"))
			print(paste("Using ",proportion," of this existing layer of data (and portions of others?)"))
			thisSection <- thisSection + proportion*volumeRLDs[j,,]			
			thisSliceVolume <- thisSliceVolume + proportion*sum(boxVolumes[j,,])
		} else if(!SBTAboveDT && SBTAboveDB && SBBBelowDT && SBBBelowDB)
		{
			proportion <- (layerbottom - sblayertop) / (sblayerbottom - sblayertop)
			print(paste("Scoreboard layer ",j,": ",sblayertop," to ",sblayerbottom," cm deep"))
			print(paste("Using ",proportion," of this existing layer of data (and portions of others?)"))
			thisSection <- thisSection + proportion*volumeRLDs[j,,]		
			thisSliceVolume <- thisSliceVolume + proportion*sum(boxVolumes[j,,])
		} else if(!SBTAboveDT && SBTAboveDB && SBBAtDB)
		{
			proportion <- (layerbottom - sblayertop) / (sblayerbottom - sblayertop)
			print(paste("Scoreboard layer ",j,": ",sblayertop," to ",sblayerbottom," cm deep"))
			print(paste("Using ",proportion," of this existing layer of data, and that's it"))
			thisSection <- thisSection + proportion*volumeRLDs[j,,]		
			thisSliceVolume <- thisSliceVolume + proportion*sum(boxVolumes[j,,])
			break
		}

	}			
	volumeRLDsByArbitraryDepth[i,,] <- thisSection

	Scoreboard_TRLD[i] <- sum(thisSection)
	print(paste("    Total Scoreboard RLD for depth ",layertop," to ",layerbottom," cm: ",Scoreboard_TRLD[i]," cm cm^-3"))

	thisTotalRootLength <- sum(thisSection) * thisSliceVolume
	Scoreboard_TRL[i] <- thisTotalRootLength
	print(paste("        SLICE VOLUME: ",thisSliceVolume,". VO VOLUME: ",VOVolumeByDepth[i], ". TOTAL ROOT LENGTH THIS SLICE: ",thisTotalRootLength))
	thisVOTRLD <- thisTotalRootLength / VOVolumeByDepth[i]
	VO_TRLD[i] <- thisVOTRLD
	print(paste("    Total restricted-to-VO RLD for depth ",layertop," to ",layerbottom," cm: ",thisVOTRLD," cm cm^-3"))
}
