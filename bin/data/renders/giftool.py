import os
import math
import subprocess

def getLength(filename):
    result = subprocess.Popen(["ffprobe", filename],
                              stdout = subprocess.PIPE, stderr = subprocess.STDOUT)
    string = [x for x in result.stdout.readlines() if "Duration" in x][0]
    s = string.split(" ")[3]
    s = s.split(":")[-1]
    s = s.replace(',','')
    s = s[1:]
    return float(s)

def getopts(argv):
    opts = {}  # Empty dictionary to store key-value pairs.
    while argv:  # While there are arguments left to parse...
        if argv[0][0] == '-':  # Found a "-name value" pair.
            opts[argv[0]] = argv[1]  # Add key and value to the dictionary.
        argv = argv[1:]  # Reduce the argument list by copying it starting from index 1.
    return opts

def copyGifMaxSize(inputFilename, maxSize, prefix):
    gifSize = os.path.getsize(inputFilename)
    attempts = 10
    hi = 256
    lo = 0

    f = inputFilename.split(".")[0]
    pf = f + prefix + ".gif"

    print(gifSize)

    if gifSize < maxSize:
        os.system("cp " + inputFilename + " " + pf)
        return

    while attempts > 0:

        colors = lo + (hi-lo)/2

        os.system("gifsicle --colors " + str(int(colors)) + " " + inputFilename + " > " +  pf)

        gifSize = os.path.getsize(pf)

        if gifSize < maxSize:
            if abs(gifSize - maxSize) < 100000:
                attempts = 0
            else:
                hi = colors+((hi-lo)/2)
                lo = colors
        else:
            hi = colors

        if gifSize >= maxSize:
            os.remove(pf)

        print( "attempt " + str(attempts) + " colors " + str(int(colors)) + " size " + str(gifSize) )
        attempts -= 1

def main(inputFilename, outputFilename):

    os.system("mkdir " + outputFilename)
    os.system("mv " + inputFilename + "*.png " + outputFilename)
    os.chdir(outputFilename)

    print("Creating gif")
    outputGifFilename = outputFilename + ".gif"
    os.system("convert +dither -resize 500X500 -delay 3 " + inputFilename + "*.png tmp.gif")
    os.system("gifsicle --colors 256 tmp.gif > " + outputGifFilename)
    print ("done!")

    print ("Making copies")
    copyGifMaxSize(outputGifFilename, 2800000, "tumblr")
    copyGifMaxSize(outputGifFilename, 4750000, "twitter")
    print ("done!")

    os.system("cd .." )

if __name__== "__main__":
    from sys import argv
    myargs = getopts(argv)
    main(myargs["-input"], myargs["-output"])
