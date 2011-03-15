#!/usr/bin/python
# -*- coding: utf-8 -*-

import sys
import wx
import wx.grid

from pyemu.realmachine import RealMachine
from pyemu.filesystem import file_system
from pyemu.memory import BLOCK_SIZE
from pyemu.memory import BLOCKS


class StdOutDialog(wx.Frame):
    def __init__(self, message, *args, **kwds):
        kwds["style"] = wx.DEFAULT_DIALOG_STYLE
        wx.Frame.__init__(self, None, -1, "")
        dial = wx.MessageDialog(None, message, "Output", wx.OK)
        result = dial.ShowModal()
        dial.Destroy()


class StdInDialog(wx.Frame):
    def __init__(self, *args, **kwds):
        kwds["style"] = wx.DEFAULT_DIALOG_STYLE
        wx.Frame.__init__(self, *args, **kwds)
        self.input_text = wx.TextCtrl(self, -1, "", style=wx.TE_PROCESS_ENTER)
        self.std_input = ""

        self.__set_properties()
        self.__do_layout()

        self.Bind(wx.EVT_TEXT_ENTER, self.Entered, self.input_text)

    def __set_properties(self):
        self.SetTitle("dialog_1")
        self.text_ctrl_1.SetMinSize((250, 25))

    def __do_layout(self):
        sizer_2 = wx.BoxSizer(wx.HORIZONTAL)
        sizer_2.Add(self.text_ctrl_1, 0, wx.ALIGN_CENTER_HORIZONTAL|wx.ALIGN_CENTER_VERTICAL, 0)
        self.SetSizer(sizer_2)
        sizer_2.Fit(self)
        self.Layout()

    def Entered(self, event):
        self.std_input = self.input_text.GetValue()

class GenericTable(wx.grid.PyGridTableBase):
    def __init__(self, data, rowLabels=None, colLabels=None):
        wx.grid.PyGridTableBase.__init__(self)
        self.data = data
        self.rowLabels = rowLabels
        self.colLabels = colLabels
        
    def GetNumberRows(self):
        return len(self.data)

    def GetNumberCols(self):
        return len(self.data[0])

    def GetColLabelValue(self, col):
        if self.colLabels:
            return self.colLabels[col]
        
    def GetRowLabelValue(self, row):
        if self.rowLabels:
            return self.rowLabels[row]
        
    def IsEmptyCell(self, row, col):
        return False

    def GetValue(self, row, col):
        if len(self.data[row]) > col:
            return self.data[row][col]
        return None

    def SetValue(self, row, col, value):
        pass         

class SimpleGridForVM(wx.grid.Grid):
    def __init__(self, parent, data_list):
        wx.grid.Grid.__init__(self, parent, -1)
        getRowLabelsForVM()
        tableBase = GenericTable(vm_data, rowLabelsForVM, colLabels)
        self.SetTable(tableBase)

class SimpleGrid(wx.grid.Grid):
    def __init__(self, parent, data_list):
        wx.grid.Grid.__init__(self, parent, -1)
        getRowLabels()
        tableBase = GenericTable(data_list, rowLabels, colLabels)
        self.SetTable(tableBase)

class MenuBar(wx.Menu):
    def __init__(self, parent):
        wx.Menu.__init__(self)

        menubar = wx.MenuBar()
        file = wx.Menu()
        quit = wx.MenuItem(file, 1, '&Quit\tCtrl+Q')
        file.AppendItem(quit)

        parent.Bind(wx.EVT_MENU, parent.OnQuit, id=1)

        menubar.Append(file, '&File')
        parent.SetMenuBar(menubar)

class VMFrame(wx.Frame):
    def __init__(self, parent, *args, **kwds):
        kwds["style"] = wx.DEFAULT_FRAME_STYLE
        wx.Frame.__init__(self, parent, -1, "Virtuali masina", size=(1366, 719))
        manu = MenuBar(self)

        self.R1_text= wx.TextCtrl(self)
        self.R1 = wx.StaticText(self, label="R1:")
        self.R2 = wx.StaticText(self, label="R2:")
        self.R2_text = wx.TextCtrl(self)
        self.SF = wx.StaticText(self, label="SF:")
        self.SF_text = wx.TextCtrl(self)
        self.IC = wx.StaticText(self, label="IC:")
        self.IC_text = wx.TextCtrl(self)
        self.R1_text.SetEditable(False)
        self.R2_text.SetEditable(False)
        self.SF_text.SetEditable(False)
        self.IC_text.SetEditable(False)

        self.grid_1 = SimpleGridForVM(self, vm_data)
        self.__set_properties()
        self.__do_layout()

    def OnQuit(self, event):
        self.Close()

    def setRegistersOnLoad(self, rm):
        self.IC_text.SetValue(str(rm.processor.registers["IC"]))

    def __set_properties(self):
        self.Maximize()
        self.grid_1.CreateGrid(39, 3)
        self.grid_1.EnableEditing(0)
        self.grid_1.EnableDragColSize(0)
        self.grid_1.SetColLabelSize(24)
        self.grid_1.SetRowLabelSize(30)
        self.grid_1.EnableDragRowSize(0)
        self.grid_1.SetMinSize((1360, 500))


    def __do_layout(self):
        sizer_1 = wx.BoxSizer(wx.VERTICAL)
        fgs = wx.FlexGridSizer(4, 8, 5, 5)
        fgs.AddMany([
          (self.R1),(self.R1_text, 0,0,0), #R1 registras
          (self.R2),(self.R2_text, 0),     #R2 registras
          (self.SF), (self.SF_text, 0),     #SF registras
          (self.IC), (self.IC_text, 0)
        ])
        sizer_1.Add(fgs, 0,wx.ALIGN_CENTER_VERTICAL, 0)
        sizer_1.Add(self.grid_1, 4, wx.ALIGN_CENTER_VERTICAL, 0)
        self.SetSizer(sizer_1)
        sizer_1.Fit(self)
        self.Layout()
        

class TestFrame(wx.Frame):
    def __init__(self, parent, *args, **kwds):
        # begin wxGlade: MyFrame1.__init__
        kwds["style"] = wx.DEFAULT_FRAME_STYLE
        wx.Frame.__init__(self, parent, -1, "Reali masina", size=(1366, 719))
        manu = MenuBar(self)
        #wx.Frame.__init__(self, *args, **kwds)
        self.R1_text= wx.TextCtrl(self)
        self.R1 = wx.StaticText(self, label="R1:")
        self.R1_text.SetEditable(False)
        self.R2 = wx.StaticText(self, label="R2:")
        self.R2_text = wx.TextCtrl(self)
        self.SF = wx.StaticText(self, label="SF:")
        self.SF_text = wx.TextCtrl(self)
        self.IC = wx.StaticText(self, label="IC:")
        self.IC_text = wx.TextCtrl(self)
        self.PLR = wx.StaticText(self, label="PLR:")
        self.PLR_text = wx.TextCtrl(self)
        self.PLBR = wx.StaticText(self, label="PLBR:")
        self.PLBR_text = wx.TextCtrl(self)
        self.CHST = wx.StaticText(self, label="CHST:")
        self.CHST_text = wx.TextCtrl(self)
        self.IOI = wx.StaticText(self, label="IOI:")
        self.IOI_text = wx.TextCtrl(self)
        self.SI = wx.StaticText(self, label="SI:")
        self.SI_text = wx.TextCtrl(self)
        self.PI = wx.StaticText(self, label="PI:")
        self.PI_text = wx.TextCtrl(self)
        self.EMPTY = wx.StaticText(self, label="")
        self.MODE_radio = wx.RadioBox(self, -1, "MODE", choices=["Vartotojas", "Supervizorius"], majorDimension=0, style=wx.RA_SPECIFY_ROWS)
        self.RUN_button = wx.Button(self, -1, "paleisti")
        self.Bind(wx.EVT_BUTTON, self.OnClick, self.RUN_button)
        self.R2_text.SetEditable(False)
        self.SF_text.SetEditable(False)
        self.IC_text.SetEditable(False)
        self.PLR_text.SetEditable(False)
        self.PLBR_text.SetEditable(False)
        self.CHST_text.SetEditable(False)
        self.IOI_text.SetEditable(False)
        self.SI_text.SetEditable(False)
        self.PI_text.SetEditable(False)

        self.grid_1 = SimpleGrid(self, data)
        self.__set_properties()
        self.__do_layout()
        # end wxGlade

    def OnClick(self, event):
        vmframe = VMFrame(self, -1, "")
        app.SetTopWindow(vmframe)
        vmframe.Centre()
        vmframe.Show()

    def OnQuit(self, event):
        self.Close()

    def setRegistersOnLoad(self, rm):
        self.IC_text.SetValue(str(rm.processor.registers["IC"]))
        self.PLR_text.SetValue(str(rm.processor.registers["PLR"]))
        self.PLBR_text.SetValue(str(rm.processor.registers["PLBR"]))

    def __set_properties(self):
        self.SetTitle("Reali masina")
        self.Maximize()
        self.grid_1.CreateGrid(39, 3)
        self.grid_1.EnableEditing(0)
        self.grid_1.EnableDragColSize(0)
        self.grid_1.SetColLabelSize(24)
        self.grid_1.SetRowLabelSize(30)
        self.grid_1.EnableDragRowSize(0)
        self.grid_1.SetMinSize((1360, 500))


    def __do_layout(self):
        sizer_1 = wx.BoxSizer(wx.VERTICAL)
        fgs = wx.FlexGridSizer(4, 8, 5, 5)
        fgs.AddMany([
          (self.R1),(self.R1_text, 0,0,0), #R1 registras
          (self.R2),(self.R2_text, 0),     #R2 registras
          (self.SF), (self.SF_text, 0),     #SF registras
          (self.IC), (self.IC_text, 0),     #SF registras
          (self.PLR), (self.PLR_text, 0),     #SF registras
          (self.PLBR), (self.PLBR_text, 0),     #SF registras
          (self.CHST), (self.CHST_text, 0),     #SF registras
          (self.IOI), (self.IOI_text, 0),     #SF registras
          (self.SI), (self.SI_text, 0),     #SF registras
          (self.PI), (self.PI_text, 0),     #SF registras
          (self.EMPTY), (self.MODE_radio),     #SF registras
          (self.EMPTY), (self.RUN_button, 0, wx.EXPAND)     #SF registras
        ])
        sizer_1.Add(fgs, 0,wx.ALIGN_CENTER_VERTICAL, 0)
        sizer_1.Add(self.grid_1, 4, wx.ALIGN_CENTER_VERTICAL, 0)
        self.SetSizer(sizer_1)
        sizer_1.Fit(self)
        self.Layout()
        

data = []
vm_data = []
colLabels = ["0","1", "2","3","4","5","6","7","8","9","A","B","C","D","E","F"]
rowLabelsForVM = []
rowLabels = []
app = wx.App()
rm = RealMachine()

def getRowLabels():
    for i in range(BLOCKS):
        rowLabels.append("%s" % i)

def getRowLabelsForVM():
    for i in range(rm.pager.get_C()):
        rowLabelsForVM.append("%s" % i)
    for i in range(150):
        rowLabelsForVM.append("%s" % i)


def stdin():
    stdin_dialog = StdInDialog(None)
    return stdin_dialog.std_input

def stdout(message):
    stdout_dialog = StdOutDialog(None, message, -1, "")

def start_gui(file):
    try:

        rm.load_virtual_machine(file, stdin, stdout)
        
        for i in range(BLOCKS):
            row = []
            for j in range(BLOCK_SIZE):
                row.append(rm.real_memory[i, j])
            data.append(row)
            
        for i in range(rm.pager.get_C()):
            row = []
            for j in range(BLOCK_SIZE):
                row.append(rm.virtual_memory_code[i, j])
            vm_data.append(row)
        for i in range(rm.pager.get_D()):
            row = []
            for j in range(BLOCK_SIZE):
                row.append(rm.virtual_memory_data[i, j])
            vm_data.append(row)
        
        frame = TestFrame(None, -1, "")
        frame.setRegistersOnLoad(rm)
        app.SetTopWindow(frame)
        frame.Centre()
        frame.Show()
    except Exception, detail:
        stdout_dialog = StdOutDialog(unicode(detail))
        return 1
    app.MainLoop()
    
