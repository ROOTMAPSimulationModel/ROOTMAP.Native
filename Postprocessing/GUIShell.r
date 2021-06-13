# GUIShell.r

# Tcl/Tk list box code adapted from http://bioinf.wehi.edu.au/~wettenhall/RTclTkExamples/listboxes.html

# This function by Joshua Ulrich, see http://stackoverflow.com/questions/4749783/how-to-obtain-a-list-of-directories-within-a-directory-like-list-files-but-in
list.dirs <- function(path=".", pattern=NULL, all.dirs=FALSE,
  full.names=FALSE, ignore.case=FALSE) {

  all <- list.files(path, pattern, all.dirs,
           full.names, recursive=FALSE, ignore.case)
  all[file.info(all)$isdir]
}

# A simple inverted version
list.nondirfiles <- function(path=".", pattern=NULL, all.dirs=FALSE,
  full.names=FALSE, ignore.case=FALSE) {

  all <- list.files(path, pattern, all.dirs,
           full.names, recursive=FALSE, ignore.case)
  all[!file.info(all)$isdir]
}

separator <- ","
fileExt <- ".csv"
plotWidth <- 1024
plotHeight <- 768
source("GUISelectConfigSubdir.r")

