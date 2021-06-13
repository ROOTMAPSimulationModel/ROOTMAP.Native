# Read in Scoreboard boundaries

scoreboardsXML = XML::xmlRoot(XML::xmlTreeParse(file.path(getwd(),configsubdir,"Scoreboards.xml")))

xBoundaryElement = XML::getNodeSet(scoreboardsXML, "/rootmap/construction/scoreboard/boundaryarray[dimension='X']/positionarray")
for(x in xBoundaryElement)	# these loops should only execute once each 
{
	xBoundaryString <- XML::xmlValue(x)
}
yBoundaryElement = XML::getNodeSet(scoreboardsXML, "/rootmap/construction/scoreboard/boundaryarray[dimension='Y']/positionarray")
for(y in yBoundaryElement)
{
	yBoundaryString <- XML::xmlValue(y)
}
zBoundaryElement = XML::getNodeSet(scoreboardsXML, "/rootmap/construction/scoreboard/boundaryarray[dimension='Z']/positionarray")
for(z in zBoundaryElement)
{
	zBoundaryString <- XML::xmlValue(z)
}

print("Loaded Scoreboard boundaries. X, Y, Z counts:")
xb <- scan(textConnection(unlist(strsplit(xBoundaryString, ","))))
yb <- scan(textConnection(unlist(strsplit(yBoundaryString, ","))))
zb <- scan(textConnection(unlist(strsplit(zBoundaryString, ","))))

zmidpoints <- NULL
for(iz in 1:(length(zb)-1))
{
	zmidpoints <- c(zmidpoints, sum(zb[iz]+(zb[iz+1]-zb[iz])/2))
}