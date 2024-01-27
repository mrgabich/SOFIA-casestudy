#!/usr/bin/python

# my imports
import sys
import os

import subprocess
import linecache
import ctypes
import string
from array import *
from enum import Enum
from optparse   import OptionParser
from random     import randint
from array      import *
import copy
import re
from operator import itemgetter
from collections import Counter

#python common class
from classification import *

# -------------------------------------- Fault Campaign options ------------------------------------- #
usage = "usage: %prog [options] arg1 arg2" # @Review
parser = OptionParser(usage=usage)

#Options and arguments

parser.add_option("--executiontime",      action="store", type="float",  dest="executiontime",     help="Execution time of the entire fault injection campaign (nanoseconds)", default=1000000000)
parser.add_option("--numberoffaults",     action="store", type="int",    dest="numberoffaults",    help="Number of faults to be injected in one platform",                     default=1)
parser.add_option("--parallelexecutions", action="store", type="int",    dest="parallelexecutions",help="Number of parallel platform executions",                              default=1)
parser.add_option("--cores",              action="store", type="int",    dest="cores",             help="Number of target cores",                                              default=1)
parser.add_option("--application",        action="store", type="string", dest="application",       help="Application name",                                                    default="application")
parser.add_option("--cputype",            action="store", type="string", dest="cputype",           help="Cpu type",                                                            default="ovp")
parser.add_option("--faultlist",          action="store", type="string", dest="faultlist",         help="File containing the fault list [default: %default]",                  default="./faultlist")
parser.add_option("--goldinformation",    action="store", type="string", dest="goldinformation",   help="File containing the golden information [default: %default]",          default="./goldinformation")
parser.add_option("--folder",             action="store", type="string", dest="folder",            help="Folder that contains the reports to be harvested",                    default="./Reports/")
parser.add_option("--dumpfolder",         action="store", type="string", dest="dumpfolder",        help="Folder that contains the dump reports to be harvested",               default="./Dumps/")
parser.add_option("--tracefolder",        action="store", type="string", dest="tracefolder",       help="Folder that contains the trace reports to be harvested",              default="./Traces/")
parser.add_option("--appdump",            action="store", type="string", dest="appdump",           help="File containing the app object dump",                                 default="./app.lst")
parser.add_option("--outputdata",         action="store", type="string", dest="outputdata",        help="Output data from an application")
parser.add_option("--outputdatasize",     action="store", type="int",    dest="outputdatasize",    help="Size of output data type in bits")
parser.add_option("--outputdataoffset",   action="store", type="int",    dest="outputdataoffset",  help="Address offset of output data type")

# architecture
parser.add_option("--environment",         type="choice", dest="environment", choices=architectures, help="Choose one architecture mode", default=architectures[0])

# simulator environment

# parse the arguments
(options, args) = parser.parse_args()

# list of possible registers
archRegisters = archRegisters()

# registers for each simulator environment
possibleRegisters = archRegisters.getListOfpossibleRegisters(options,parser)
numberOfRegisters = archRegisters.getNumberOfRegisters()

####################################################################################################
# Fault Harvest                                                                                    #
####################################################################################################
faultList=[]

# expected number of faults
numberTotalOfFaults = options.numberoffaults * options.parallelexecutions

instructionCount = int(subprocess.check_output('grep "Application # of instructions" {0} | cut -d \'=\' -f2'.format(options.goldinformation),shell=True,).rstrip())

numberApplicationsPlotted = 0

# collect all faults
for filename in os.listdir(options.folder):
    #~ print filename
    numberApplicationsPlotted+=1
    # Collect the faults
    for i in range(2,options.numberoffaults+2):
        if not linecache.getline(options.folder+filename,i) == '': # skip empty lines
            faultList.append(linecache.getline(options.folder+filename,i).split())
        else:
            numberApplicationsPlotted-=1

# sort the list by fault index
faultList.sort(key=lambda x: int(x[0]))

# see if all fault reports are accessible
if(numberApplicationsPlotted != options.parallelexecutions):
    print ("error - missing files " + str(numberTotalOfFaults-numberApplicationsPlotted))

# the files missing are considered as hangs in the ovp and unexpected termination in the gem5
if (numberApplicationsPlotted != options.parallelexecutions):
    a=0

    # collect the faults from the fault list
    for i in range(1,numberTotalOfFaults+1):
        if int(faultList[a][0]) == i:
            a+=(1 if a<(len(faultList)-1) else 0)
        else: # include missing faults
            if options.environment==archtecturesE.gem5armv7.name or options.environment==archtecturesE.gem5armv8.name:
                temp =str(linecache.getline(options.faultlist,i+1).rstrip()+" "+str(errorAnalysis.RD_PRIV.name)+" "+str(errorAnalysis.RD_PRIV.value)+" 0 "+str(instructionCount)+" Null 0")
                faultList.append(temp.split())
                numberApplicationsPlotted+=1
                faultList[-1][4] = faultList[-1][4].split(":")[0]
            else:
                temp =str(linecache.getline(options.faultlist,i+1).rstrip()+" "+str(errorAnalysis.Hang.name)+" "+str(errorAnalysis.Hang.value)+" 0 "+str(instructionCount)+" Null 0 ERROR")
                faultList.append(temp.split())
                numberApplicationsPlotted+=1
                faultList[-1][4] = faultList[-1][4].split(":")[0]

# -------------------------------------- Making some calculation ------------------------------------ #
# total number of REALLY executed instructions
totalExecutedInstructions=int(sum([int(item[9]) for item in faultList]))
mipsExecutedInstructions =float(totalExecutedInstructions)/(options.executiontime /1000)

totalExecutedInstructionsWithChkp=0
for fault in faultList:
    if(fault[9]=="0"):
        print (fault)
    totalExecutedInstructionsWithChkp += int(fault[11]) + int(fault[9])

mipsExecutedInstructionsWithChkp =float(totalExecutedInstructionsWithChkp)/(options.executiontime /1000)

# --------------------------------------- Verify the Output Data ------------------------------------ #
if options.outputdata:
    #Auxiliar structures
    gold_appdata=[]
    fault_appdata=[]
    output_start = 0
    
    #Find the Address in the Gold Trace Dump
    found=0
    goldPatternIndex=0
    #insert here the output data offset
    Lines=open(options.tracefolder+"GOLD_TRACE-0").readlines()
    for line in Lines:
        gold_appdata.append(line.split()[0])
    #Remove empty places
    while("" in gold_appdata):
        gold_appdata.remove("")
    
    #print(faultList)
    #Compares Output from Gold with Fault Variable Trace
    for i in range(0,len(faultList)):
        fault_appdata.clear()
        found=0
        dpfile=str(options.tracefolder+"FAULT_TRACE-"+faultList[i][0])
        if os.path.isfile(dpfile) and os.stat(dpfile).st_size > 0:
            Lines=open(dpfile).readlines()
            for line in Lines:
                fault_appdata.append(line.split()[0])
            os.remove(dpfile)
            #Remove empty places
            while("" in fault_appdata):
                fault_appdata.remove("")
            #If the exit is identical it assumes that it is correct
            if not (set(gold_appdata)-set(fault_appdata)):
                faultList[i][12] = "CORRECT"
                #print("entrou")
            else:
                faultList[i][12] = "WRONG"
                #print("e aqui?")
            #create lists with a hexa word (gold and fault dump)    
            data_size_offset = int(options.outputdatasize)
            #gold list    
            goldresultlist = []
            for x in range(0, len(gold_appdata)):
                for index in range(0, len(gold_appdata[x]), data_size_offset):
                    hex_word = gold_appdata[x][index : index + data_size_offset]
                    goldresultlist.append(hex_word)
            #fault list
            faultresultlist = []
            for x in range(0, len(fault_appdata)):
                for index in range(0, len(fault_appdata[x]), data_size_offset):
                    hex_word = fault_appdata[x][index : index + data_size_offset]
                    faultresultlist.append(hex_word)
            #printlist
            #x=0
            #for z in goldresultlist:
                #print("{}\t\t{}".format(goldresultlist[x], faultresultlist[x]))
# -------------------------------------- Generate the Output File ----------------------------------- #
fileptr = open(options.application+"."+options.environment+".reportfile", 'w')

# fileptr.write("%8s\n"%(len(faultList)))

# header file
fileptr.write("%8s %14s %10s %4s %18s %130s %28s %7s %25s %22s %18s %15s %15s\n"%(
        "Index","Type","Target","i","Insertion Time","Mask","Fault Injection Result",
        "Code", "Execution Time (Ticks)", "Executed Instructions", "Mem Inconsistency",
        "Checkpoint", "Trace Variable"))

# print faults
for faultNumber in range(0,len(faultList)):
    faultIndex                  = faultList[faultNumber][0]
    faultType                   = faultList[faultNumber][1]
    faultRegister               = faultList[faultNumber][2]
    faultRegisterIndex          = faultList[faultNumber][3]
    faultTime                   = faultList[faultNumber][4]
    faultMask                   = faultList[faultNumber][5]
    faultAnalysis               = faultList[faultNumber][6]
    faultAnalysisCode           = faultList[faultNumber][7]
    faultTicks                  = faultList[faultNumber][8]
    faultExecutedInstructions   = faultList[faultNumber][9]
    faultMemInconsistency       = faultList[faultNumber][10]
    faultCorrectCheckpoint      = faultList[faultNumber][11]
    faultTraceVariable          = faultList[faultNumber][12]

    fileptr.write("%8s %14s %10s %4s %18s %130s %28s %7s %25s %22s %18s %15s %15s\n" % (
                    faultIndex,
                    faultType,
                    faultRegister,
                    faultRegisterIndex,
                    faultTime,
                    faultMask,
                    faultAnalysis,
                    faultAnalysisCode,
                    faultTicks,
                    faultExecutedInstructions,
                    faultMemInconsistency,
                    faultCorrectCheckpoint,
                    faultTraceVariable
                    ))

spacing = 40

# additional information @Review
fileptr.write("\n\n")
fileptr.write("%s %s\n" %("Application".ljust(spacing),options.application))
fileptr.write("%s %d\n" %("Recovered faults".ljust(spacing),numberTotalOfFaults))
fileptr.write("%s %d\n" %("Target Cores".ljust(spacing),options.cores))
fileptr.write("%s %s\n" %("Environment".ljust(spacing),options.environment))
fileptr.write("%s %s\n" %("CPU type".ljust(spacing),options.cputype))

fileptr.write("%s %d\n" %("Executed Instructions".ljust(spacing),totalExecutedInstructions))
fileptr.write("%s %d\n" %("Total Emulated".ljust(spacing),totalExecutedInstructionsWithChkp))
fileptr.write("%s %d\n" %("Gold Executed Instructions".ljust(spacing),int(instructionCount)))

fileptr.write("%s %.3f\n" %("Simulation Time (seconds)".ljust(spacing),options.executiontime/1000000000.0))
fileptr.write("%s %.3f\n" %("Million instructions per second (MIPS)".ljust(spacing),mipsExecutedInstructions))
fileptr.write("%s %.3f\n" %("MIPS if with checkpoint".ljust(spacing),mipsExecutedInstructionsWithChkp))

fileptr.close()