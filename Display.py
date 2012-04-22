###############################################
#####                                     #####
#####                DISPLAY              #####
#####                                     #####
#####         AUTHOR: DUSTIN TAUXE        #####
#####                                     #####
#####                USAGE                #####
#####                                     #####
##### -a : disable window size adjustment #####
#####      (not recommended)              #####
##### -g : disable graph display          #####
##### -f [arg] : input file name          #####
#####      (default="GUIFile")            #####
###############################################

import pygame
import sys
from os.path import exists
from os import remove
from time import time

class Graph(): # Class for making a graph of the ACO route
  def __init__(self):
    
    self.Xsize = 1024           # Window X size, in pixels; defualt value
    self.Ysize = 768            # Window Y size, in pixels; defualt value
    self.graphYsize = 100       # Graph Y size, in pixels
    
    self.gmax = 0
    self.gmin = 1234567890
    
    self.bgcolor = (0, 0, 0)        # Background color
    self.ctcolor = (0,255,0)        # City color
    self.trcolor = (255, 255, 255)  # Tour color
    self.txcolor = (255, 255, 255)  # Text color
    self.tbcolor = (0, 0, 0)        # Text background color
    self.gbcolor = (255, 255, 255)  # Graph background color
    self.gicolor = (255, 50, 50)    # Graph Iter best color
    self.ggcolor = (50, 50, 255)    # Graph Glob best color
    self.gscolor = (0, 0, 0)        # Graph seperator color
    self.glcolor = (0, 0, 0)        # Graph label color
    self.gacolor = (0, 0, 0)        # Graph axes colors
    
    self.txtSize = 24       # Value for the size of the readout text displayed
    self.gAxesTxtSize = 18  # size of text on graph axes
    self.iterBest = []      # array of Iteration best tours
    self.globBest = []      # array of Global best tours
    self.elapsedTime = 0    # Time since it started
    
    self.refreshRate = 5        # Graph refresh rate (in frames/second)
    
    self.cities = []            # List of all cities in this problem set
    self.tour = None            # List by city index of tour
    self.TSPName = ""           # Problem name
    self.input = ""             # input line from file
    self.iterNum = -1           # how many iterations
    
    self.Xmin = 0     # Bounds for grid
    self.Xmax = 0     # ...
    self.Ymin = 0     # ...
    self.Ymax = 0     # ...
    
    self.WinPixBuffer = 10          # Pixel buffer btwn side of window, and farthest point. Purely asthetic.
    self.graphPixBuffer = 10        # Pixel buffer for graph edges
    
    self.AdjustWindow = True        # Allow adjustment of window size to fit problem
    self.displayText = True         # Display text overlay
    self.displayTour = True         # Display tour
    self.displayCity = True         # Display cities
    self.displayGraphs = True       # Display iterbest and globbest tour length graphs
    self.graphTop = False           # True if graphs on top, false if on bottom
    self.displayGraphLabels = True  # Display text labels for graphs
    self.displayGraphBounds = True  # Display bounds on graphs
    
    self.fname = "GUIFile"                # Input file it looks for
    
    self.deleteFile = True               # Delete input file when done
    self.forceCreate = True              # Force creation of new input file
    
    pygame.init()							# Initialize the pygame module
    
    for i in range(1, len(sys.argv)):
      option = sys.argv[i]
      
      if option == "-a":
        self.AdjustWindow = False
        continue
      if option == "-g":
        self.displayGraphs = False
        continue
      if option == "-f":
        self.fname = sys.argv[i+1]
        continue
  
  def tuples_to_lists(self,l):
    return [[x,y] for (x,y) in l]

  def run(self): 								# Display function
    if not exists(self.fname) or self.forceCreate:
      f = open(self.fname, 'w')
      f.close()
      #self.deleteFile = True

    # Main Loop
    isInitialized = False
    while True:
      for event in pygame.event.get():  # This exits program on QUIT event (Clicking the close button)
        if event.type == pygame.QUIT:
          self.close()
      
      f = open(self.fname, 'r')
      # If the data has been put in file:
      if self.hasData(f):
        # If first time
        if not isInitialized:
          self.readEarlyData()
          start = time()
          isInitialized = True
          self.findBounds()       # Do initial operations for graphing
          self.translatePoints()  # ...
          Window = pygame.display.set_mode((self.Xsize,self.Ysize))	# Frame
          Canvas = pygame.Surface(Window.get_size()) 					# For drawing on, fills frame
          Clock = pygame.time.Clock() 								# Used to time refreshes
          Text = pygame.font.Font(None, self.txtSize)
          pygame.display.set_caption("ACO Output")
          if self.displayText:
            pygame.font.init()  # Initialize pygame's font module
          else:
            self.txtSize = 0
          if self.displayGraphs:
            gCanvas = pygame.Surface((self.Xsize, self.graphYsize), 0, Canvas)
            if self.displayGraphLabels:
              labelText = pygame.font.Font(None, self.gAxesTxtSize)
            else:
              self.gAxesTxtSize = 0
          else: 
            self.graphYSize = 0

        # Every time: Read data, update time
        if self.readData():
          self.elapsedTime = time() - start

          Clock.tick(self.refreshRate)    # Enforces slower tick rate
          Canvas.fill(self.bgcolor)       # Color background
          
          # Draw stuff on Canvas here:
          
          #$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
          # Display CITIES
          if self.displayCity:
            # Draw the cities
            for city in self.cities:
              pygame.draw.circle(Canvas, self.ctcolor, (city[0], city[1]), 3)
          
          #$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
          # Display TOUR
          if self.displayTour and self.tour != None:
            # Create list of points visited in tour
            ptlist = []
            for point in self.tour:
              try:
                ptlist.append(self.cities[point])
              except IndexError:
                pass
            # Draw the lines
            pygame.draw.lines(Canvas, self.trcolor, True, ptlist, 1)
          
          #$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
          # Display GRAPHS
          if self.displayGraphs:
            gCanvas.fill(self.gbcolor)

            temp = self.Xsize/2
            pygame.draw.line(gCanvas, self.gscolor, (temp,0), (temp,self.graphYsize), 3)
            
            #Draw IterBest graph
            if self.displayGraphLabels:
              txtSurface = Text.render("Iteration Best", True, self.glcolor)
              txtRect = txtSurface.get_rect()
              gCanvas.blit(txtSurface, txtRect)
              
              txtSurface = labelText.render("Tour Length", True, self.glcolor)
              txtSurface = pygame.transform.rotate(txtSurface, 90)
              txtRect = (0, self.txtSize, txtSurface.get_width(), txtSurface.get_height())
              gCanvas.blit(txtSurface, txtRect)
              
              adjust = self.graphPixBuffer
              if self.displayGraphBounds:
                # Y axis
                txtSurface = labelText.render("%.0f"%self.gmax, True, self.glcolor)
                txtSurface = pygame.transform.rotate(txtSurface, 90)
                txtRect = (self.gAxesTxtSize, self.txtSize-7, txtSurface.get_width(), txtSurface.get_height())
                gCanvas.blit(txtSurface, txtRect)
                
                txtSurface = labelText.render("%.0f"%self.gmin, True, self.glcolor)
                txtSurface = pygame.transform.rotate(txtSurface, 90)
                txtRect = (self.gAxesTxtSize, self.graphYsize - self.graphPixBuffer - txtSurface.get_height(), txtSurface.get_width(), txtSurface.get_height())
                gCanvas.blit(txtSurface, txtRect)
                
                # X axis
                txtSurface = labelText.render("0", True, self.glcolor)
                txtRect = (self.gAxesTxtSize+12, self.graphYsize-txtSurface.get_height(), txtSurface.get_width(), txtSurface.get_height())
                gCanvas.blit(txtSurface, txtRect)
                
                txtSurface = labelText.render("%.0f"%self.elapsedTime, True, self.glcolor)
                txtRect = (temp-txtSurface.get_width()-self.graphPixBuffer, self.graphYsize-txtSurface.get_height(), txtSurface.get_width(), txtSurface.get_height())
                gCanvas.blit(txtSurface, txtRect)
                
                adjust = self.gAxesTxtSize

                txtSurface = labelText.render("Time (sec)", True, self.glcolor)
                txtRect = ((temp-txtSurface.get_width())/2, self.graphYsize-txtSurface.get_height(), txtSurface.get_width(), txtSurface.get_height())
                gCanvas.blit(txtSurface, txtRect)
                
            pygame.draw.line(gCanvas, self.gacolor, (2*self.gAxesTxtSize, self.gAxesTxtSize), (2*self.gAxesTxtSize, self.graphYsize-adjust))
            pygame.draw.line(gCanvas, self.gacolor, (2*self.gAxesTxtSize, self.graphYsize-adjust), (temp-self.graphPixBuffer, self.graphYsize-adjust))
            
            self.drawGraph(gCanvas, (self.gAxesTxtSize, 0), (temp-self.graphPixBuffer, self.graphYsize), self.iterBest, self.gicolor)
            
            #Draw GlobBest graph
            if self.displayGraphLabels:
              txtSurface = Text.render("Global Best", True, self.glcolor)
              txtRect = (temp+2, 0, txtSurface.get_width(), self.txtSize)
              gCanvas.blit(txtSurface, txtRect)
              
              txtSurface = labelText.render("Tour Length", True, self.glcolor)
              txtSurface = pygame.transform.rotate(txtSurface, 90)
              txtRect = (temp+2, self.txtSize, txtSurface.get_width(), txtSurface.get_height())
              gCanvas.blit(txtSurface, txtRect)
              
              adjust = self.graphPixBuffer
              if self.displayGraphBounds:
                # Y axis
                txtSurface = labelText.render("%.0f" % self.gmax, True, self.glcolor)
                txtSurface = pygame.transform.rotate(txtSurface, 90)
                txtRect = (temp+2 + self.gAxesTxtSize, self.txtSize-7, txtSurface.get_width(), txtSurface.get_height())
                gCanvas.blit(txtSurface, txtRect)
                
                txtSurface = labelText.render("%.0f" % self.gmin, True, self.glcolor)
                txtSurface = pygame.transform.rotate(txtSurface, 90)
                txtRect = (temp+2 + self.gAxesTxtSize, self.graphYsize - self.graphPixBuffer - txtSurface.get_height(), txtSurface.get_width(), txtSurface.get_height())
                gCanvas.blit(txtSurface, txtRect)
                
                # X axis
                txtSurface = labelText.render("0", True, self.glcolor)
                txtRect = (temp+self.gAxesTxtSize+12, self.graphYsize-txtSurface.get_height(), txtSurface.get_width(), txtSurface.get_height())
                gCanvas.blit(txtSurface, txtRect)
                
                txtSurface = labelText.render("%.0f"%self.elapsedTime, True, self.glcolor)
                txtRect = (gCanvas.get_width()-txtSurface.get_width()-self.graphPixBuffer, self.graphYsize-txtSurface.get_height(), txtSurface.get_width(), txtSurface.get_height())

                gCanvas.blit(txtSurface, txtRect)

                adjust = self.gAxesTxtSize
		
                txtSurface = labelText.render("Time (sec)", True, self.glcolor)
                txtRect = (self.Xsize-(temp+txtSurface.get_width())/2, self.graphYsize-txtSurface.get_height(), txtSurface.get_width(), txtSurface.get_height())
                gCanvas.blit(txtSurface, txtRect)
		
            # Axes
            pygame.draw.line(gCanvas, self.gacolor, (temp+2*self.gAxesTxtSize, self.gAxesTxtSize), (temp+2*self.gAxesTxtSize, self.graphYsize-adjust))
            pygame.draw.line(gCanvas, self.gacolor, (temp+2*self.gAxesTxtSize, self.graphYsize-adjust), (gCanvas.get_width()-self.graphPixBuffer, self.graphYsize-adjust))
            # Plot line
            self.drawGraph(gCanvas, (temp + self.gAxesTxtSize, 0), (self.Xsize-self.graphPixBuffer, self.graphYsize), self.globBest, self.ggcolor)
            
            if self.graphTop:
              temp = 0
            else:
              temp = self.Ysize - self.graphYsize
            
            Canvas.blit(gCanvas, (0, temp))
          
          #$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
          # Display TEXT
          if self.displayText:
            rtime = "%.3f" % self.elapsedTime
            iter = "%.3f" % self.iterBest[-1]
            glob = "%.3f" % self.globBest[-1]
            strout = "Problem Name: " + self.TSPName + "    Iteration Best: " + iter + "    Global Best: " + glob + "    Time: " + rtime
            txtSurface = Text.render(strout, True, self.txcolor, self.tbcolor)	# Create surface to display text on
            #textRect = txtSurface.get_rect()
            if self.graphTop:
              graphOffset = self.graphYsize
            else:
              graphOffset = 0
            txtRect = pygame.Rect(0, graphOffset, self.Xsize, self.txtSize)
            bgRect = pygame.Rect(0, graphOffset, self.Xsize, self.txtSize)
            pygame.draw.rect(Canvas, self.tbcolor, bgRect)
            Canvas.blit(txtSurface, txtRect)	# Draw text surface onto canvas
          
          #$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
          
          Window.blit(Canvas, (0,0))	# Draw canvas in buffer
          pygame.display.flip()		# Display buffer
    f.close()

  # Canvas to draw on, (topleft corner), (bottom right corner), list of values, color of line)
  def drawGraph(self, Canvas, (x, y), (xmax, ymax), l, color):
    pts = []
    if self.displayGraphLabels:
      y += self.txtSize - 10
      ymax -= self.txtSize - 15
    xdist = (xmax-x - 2*self.graphPixBuffer)/float(len(l))
    height = ymax - y - 2*self.graphPixBuffer
    count = 0
    for val in l:
      xDraw = self.gAxesTxtSize + x + (count * xdist)
      #if self.displayGraphLabels:  
      #  y += self.txtSize
      yDraw = self.graphPixBuffer + y + height * ((self.gmax - val) / (self.gmax - self.gmin + 0.000000000001))
      pts.append((xDraw,yDraw))

      count += 1
    if count > 1:
      pygame.draw.lines(Canvas, color, False, pts, 1)
  
  def findBounds(self): # Finds X/Y min/max
    
    Xmin = self.cities[0][0]
    Ymin = self.cities[0][1]
    Xmax = 0
    Ymax = 0
    
    for city in self.cities:
      if city[0] < Xmin:
        Xmin = city[0]
      if city[0] > Xmax:
        Xmax = city[0]
      if city[1] < Ymin:
        Ymin = city[1]
      if city[1] > Ymax:
        Ymax = city[1]
    
    self.Xmin = Xmin
    self.Xmax = Xmax
    self.Ymin = Ymin
    self.Ymax = Ymax

  def translatePoints(self): # Translates points in cities to cooresponding pixels
    
    Xlen = self.Xmax - self.Xmin
    Ylen = self.Ymax - self.Ymin
    
    Xratio = (self.Xsize - 2*self.WinPixBuffer)/Xlen
    Yratio = (self.Ysize - (self.WinPixBuffer + self.txtSize))/Ylen
    ConversionRatio = 0
    
    if Xratio <= Yratio:
      ConversionRatio = Xratio
    else:
      ConversionRatio = Yratio
    
    for city in self.cities:
      city[0] = int((city[0] - self.Xmin)*ConversionRatio)
      city[1] = int((city[1] - self.Ymin)*ConversionRatio)
      # Y = 0 is the top, so this flips points to match
      city[1] = int((self.Ymax - self.Ymin)*ConversionRatio - city[1])


      if self.graphTop:
        graphOffset = self.graphYsize
      else:
        graphOffset = 0
      city[0] = city[0] + self.WinPixBuffer
      city[1] = city[1] + (self.WinPixBuffer + self.txtSize + graphOffset)
    
    if self.AdjustWindow:
      self.Xsize = int(Xlen*ConversionRatio + 2*self.WinPixBuffer)
      self.Ysize = int(Ylen*ConversionRatio + self.txtSize + 2*self.WinPixBuffer + self.graphYsize)
      self.AdjustWindow = False

  def toRoutes(self, ptlist):	# Given a set of points, will return a list of routes that those points make
    routes = []
    index = 1
    while index < len(ptlist):
      routes.append([ptlist[index-1], ptlist[index]])
      index = index + 1
    return routes
  
  #######################################
  # Functions for reading data          #
  #######################################
  def readData(self):
    prevIterNum = self.iterNum
    temp = self.input.split(':')
    self.iterNum = int(temp[5])
    if prevIterNum != self.iterNum: # file update has occured
      temp2 = temp[2].split(',')
      self.tour = []
      for i in range(len(temp2)-1):
        self.tour.append(int(temp2[i]))
      
      self.iterBest.append(float(temp[3]))
      self.globBest.append(float(temp[4]))
      
      if float(temp[3]) > self.gmax:
        self.gmax = float(temp[3])
      if float(temp[3]) < self.gmin:
        self.gmin = float(temp[3])
      if float(temp[4]) > self.gmax:
        self.gmax = float(temp[4])
      if float(temp[4]) < self.gmin:
        self.gmin = float(temp[4])
      return True
      
    return False

  def hasData(self, f):
    result = False
    for line in f:
      self.input = line
      result = True
    return result

  def readEarlyData(self):
    TSPFile = self.input.split(':')[0]
    self.TSPName = self.input.split(':')[1]
    t = open(TSPFile, 'r')
    coord_mode = False
    for line in t:
      content = line.split()
      if (len(content) == 1):
        if content[0] == "EOF":
          coord_mode = False
      if (coord_mode and (len(content) == 3)):
        x = float(content[1])
        y = float(content[2])
        self.cities = self.cities + [[x,y]]
      if (len(content) == 1):
        if content[0] == "NODE_COORD_SECTION":
          coord_mode = True
    t.close()
    return
  
  def close(self):
    if self.deleteFile:
      try:
        remove(self.fname)
      except WindowsError:
        print "Unable to delete input file: '" + self.fname + "'"
    
    sys.exit(0)
### END OF CLASS ########################
### MAIN ################################

g = Graph()
g.run()

#Copyright (c) 2012, Dustin Tauxe
#All rights reserved.
#
#Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
#
#    Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
#    Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
#    Neither the name of Excellants nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
#
#THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
