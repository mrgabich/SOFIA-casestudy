#My imports
import sys

import subprocess
import linecache
import ctypes

from optparse   import OptionParser
from random     import randint
from array      import *

from array import *
from enum import Enum

#List of possible registers
possibleRegisters=[]

class archRegisters:
    def __init__(self):
        self.possibleRegisters=0
        self.listOfpossibleRegisters=[]

    def getListOfpossibleRegisters(self,options,parser):
        if options.environment==archtecturesE.ovparmv7.name:
            self.listOfpossibleRegisters = ["r0","r1","r2","r3","r4","r5","r6","r7","r8","r9","r10","r11","r12","sp","lr","pc","d0","d1","d2","d3","d4","d5","d6","d7","d8","d9","d10","d11","d12","d13","d14","d15"]
            self.floatRegisters          = ["d0","d1","d2","d3","d4","d5","d6","d7","d8","d9","d10","d11","d12","d13","d14","d15"]
            self.busWidth=32
        elif options.environment==archtecturesE.gem5armv7.name:
            self.listOfpossibleRegisters = ["r0","r1","r2","r3","r4","r5","r6","r7","r8","r9","r10","r11","r12","r13","r14","pc"]
            self.busWidth=32
        elif options.environment==archtecturesE.ovparmv8.name:
            self.listOfpossibleRegisters = ["x0","x1","x2","x3","x4","x5","x6","x7","x8","x9","x10","x11","x12","x13","x14","x15","x16","x17","x18","x19","x20","x21","x22","x23","x24","x25","x26","x27","x28","x29","x30","sp","pc","v0","v1","v2","v3","v4","v5","v6","v7","v8","v9","v10","v11","v12","v13","v14","v15","v16","v17","v18","v19","v20","v21","v22","v23","v24","v25","v26","v27","v28","v29","v30","v31","z0","z1","z2","z3","z4","z5","z6","z7","z8","z9","z10","z11","z12","z13","z14","z15","z16","z17","z18","z19","z20","z21","z22","z23","z24","z25","z26","z27","z28","z29","z30","z31","p0","p1","p2","p3","p4","p5","p6","p7","p8","p9","p10","p11","p12","p13","p14","p15","FFR"]
            self.gpRegisters            = ["x0","x1","x2","x3","x4","x5","x6","x7","x8","x9","x10","x11","x12","x13","x14","x15","x16","x17","x18","x19","x20","x21","x22","x23","x24","x25","x26","x27","x28","x29","x30","sp","pc"]
            self.floatRegisters          = ["v0","v1","v2","v3","v4","v5","v6","v7","v8","v9","v10","v11","v12","v13","v14","v15","v16","v17","v18","v19","v20","v21","v22","v23","v24","v25","v26","v27","v28","v29","v30","v31"]
            self.vectorRegisters         = ["z0","z1","z2","z3","z4","z5","z6","z7","z8","z9","z10","z11","z12","z13","z14","z15","z16","z17","z18","z19","z20","z21","z22","z23","z24","z25","z26","z27","z28","z29","z30","z31","p0","p1","p2","p3","p4","p5","p6","p7","p8","p9","p10","p11","p12","p13","p14","p15","FFR"]
            self.busWidth=64
        elif options.environment==archtecturesE.gem5armv8.name:
            self.listOfpossibleRegisters = ["x0","x1","x2","x3","x4","x5","x6","x7","x8","x9","x10","x11","x12","x13","x14","x15","x16","x17","x18","x19","x20","x21","x22","x23","x24","x25","x26","x27","x28","x29","x30","sp","pc"]
            self.busWidth=64
        elif options.environment==archtecturesE.ovpmips.name:
            self.listOfpossibleRegisters = ["at","v0","v1","a0","a1","a2","a3","t0","t1","t2","t3","t4","t5","t6","t7","s0","s1","s2","s3","s4","s5","s6","s7","t8","t9","k0","k1","gp","sp","s8","ra","pc"]
            self.busWidth=32
        elif (options.environment==archtecturesE.riscv32.name or options.environment==archtecturesE.riscv64.name):
            self.listOfpossibleRegisters = ["ra","sp","gp","tp","t0","t1","t2","s0","s1","a0","a1","a2","a3","a4","a5","a6","a7","s2","s3","s4","s5","s6","s7","s8","s9","s10","s11","t3","t4","t5","t6","pc","ft0","ft1","ft2","ft3","ft4","ft5","ft6","ft7","fs0","fs1","fa0","fa1","fa2","fa3","fa4","fa5","fa6","fa7","fs2","fs3","fs4","fs5","fs6","fs7","fs8","fs9","fs10","fs11","ft8","ft9","ft10","ft11","v0","v1","v2","v3","v4","v5","v6","v7","v8","v9","v10","v11","v12","v13","v14","v15","v16","v17","v18","v19","v20","v21","v22","v23","v24","v25","v26","v27","v28","v29","v30","v31"]
            self.gpRegisters             = ["ra","sp","gp","tp","t0","t1","t2","s0","s1","a0","a1","a2","a3","a4","a5","a6","a7","s2","s3","s4","s5","s6","s7","s8","s9","s10","s11","t3","t4","t5","t6","pc"]
            self.floatRegisters          = ["ft0","ft1","ft2","ft3","ft4","ft5","ft6","ft7","fs0","fs1","fa0","fa1","fa2","fa3","fa4","fa5","fa6","fa7","fs2","fs3","fs4","fs5","fs6","fs7","fs8","fs9","fs10","fs11","ft8","ft9","ft10","ft11"]
            self.vectorRegisters         = ["v0","v1","v2","v3","v4","v5","v6","v7","v8","v9","v10","v11","v12","v13","v14","v15","v16","v17","v18","v19","v20","v21","v22","v23","v24","v25","v26","v27","v28","v29","v30","v31"]
            if options.environment==archtecturesE.riscv32.name:
                self.busWidth=32
            elif options.environment==archtecturesE.riscv64.name:
                self.busWidth=64
            else:
                self.busWidth=32
        else:
            parser.error("One environment is required")

        self.numberOfRegisters = len(self.listOfpossibleRegisters)-1

        return self.listOfpossibleRegisters

    def getNumberOfRegisters(self):
        return (self.numberOfRegisters+1)

class errorAnalysis(Enum):
    Masked_Fault,\
    Control_Flow_Data_OK,\
    Control_Flow_Data_ERROR,\
    REG_STATE_Data_OK,\
    REG_STATE_Data_ERROR,\
    silent_data_corruption,\
    silent_data_corruption2,\
    silent_data_corruption3,\
    RD_PRIV,\
    WR_PRIV,\
    RD_ALIGN,\
    WR_ALIGN,\
    FE_PRIV,\
    FE_ABORT,\
    SEG_FAULT,\
    ARITH,\
    Hard_Fault,\
    Lockup,\
    Unknown,\
    Hang = list(range(20))

class errorAnalysisDAC(Enum):
    Vanished,\
    Application_Output_Not_Affected,\
    Application_Output_Mismatch,\
    Unexpected_Termination,\
    Hang = list(range(5))

class errorAnalysisDACShort(Enum):
    Vanish,\
    ONA,\
    OMM,\
    UT,\
    Hang = list(range(5))

class errorAnalysisNNShort(Enum):
    CORRECT,\
    INPROB1,\
    INPROB2,\
    WRONG,\
    NOPRED,\
    ERROR = list(range(6))

class errorAnalysisML(Enum):
    Correct,\
    Incorrect_Probability_Soft,\
    Incorrect_Probability_Hard,\
    Wrong_Probability,\
    No_Prediction,\
    UT_Hang = list(range(6))

class simulationStatesE (Enum):
    FIM_GOLD_EXECUTION,\
    FIM_GOLD_PROFILING,\
    FIM_BEFORE_INJECTION,\
    FIM_AFTER_INJECTION,\
    FIM_WAIT_HANG,\
    FIM_CREATE_FAULT,\
    FIM_NO_INJECTION = list(range(7))

class faultTypesE(Enum):
    register,\
    memory,\
    robsource,\
    robdest,\
    variable,\
    functiontrace = list(range(6))

class archtecturesE(Enum):
    riscv32,        \
    ovparmv7,       \
    ovparmv8,       \
    riscv64,        \
    ovpmips,        \
    gem5armv7,      \
    gem5armv8 = list(range(7))

faultTypes    = [item.name for item in faultTypesE]
architectures = [item.name for item in archtecturesE]
