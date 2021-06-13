# Calculate volumes of each box in the Scoreboard volume

boxVolumes <- array( ,c(length(zb)-1,length(xb)-1,length(yb)-1))

for(nx in 1:(length(xb)-1))
{		
	for(nz in 1:(length(zb)-1))
	{
		for(ny in 1:(length(yb)-1))
		{
			xdim <- sum(xb[nx+1]-xb[nx])
			ydim <- sum(yb[ny+1]-yb[ny])
			zdim <- sum(zb[nz+1]-zb[nz])
			vol <- xdim*ydim*zdim
			boxVolumes[nz,nx,ny] <- vol
		}
	}
}

print("Computed Scoreboard box volumes.")