# Calculate surface areas (in cm^2) of each box in the Scoreboard volume
# Needless to say, all boxes with the same Z-value have the same surface area

boxSurfaceAreas <- array( ,c(length(xb)-1,length(yb)-1))

for(nx in 1:(length(xb)-1))
{		
	for(ny in 1:(length(yb)-1))
	{
			xdim <- sum(xb[nx+1]-xb[nx])
			ydim <- sum(yb[ny+1]-yb[ny])
			boxSurfaceAreas[nx,ny] <- xdim*ydim
	}
}

print("Computed Scoreboard box surface areas.")