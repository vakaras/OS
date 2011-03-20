#!/usr/bin/python
# -*- coding: utf-8 -*-

import sys
import wx
import wx.grid

from pyemu.realmachine import RealMachine
from pyemu.filesystem import file_system
from pyemu.memory import BLOCK_SIZE
from pyemu.memory import BLOCKS

from pyemu import exceptions
from pyemu.exceptions import StopProgram


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
        wx.Frame.__init__(self, None, -1, "")
        dialog = wx.TextEntryDialog( None, 'INPUT:', 'Input dialog', '' )
        result = dialog.ShowModal()
        if result == wx.ID_OK:
            self.std_input = dialog.GetValue()
        else:
            self.std_input = ""
        dialog.Destroy()

class StdInDialog_old(wx.Frame):
    def __init__(self, *args, **kwds):
        kwds["style"] = wx.DEFAULT_DIALOG_STYLE
        wx.Frame.__init__(self, *args, **kwds)
        self.input_text = wx.TextCtrl(self,-1, "", style=wx.TE_PROCESS_ENTER)
        self.std_input = ""

        self.__set_properties()
        self.__do_layout()

        self.Bind(wx.EVT_TEXT_ENTER, self.Entered, self.input_text)

    def __set_properties(self):
        self.SetTitle("INPUT:")
        self.input_text.SetMinSize((250, 25))

    def __do_layout(self):
        sizer_2 = wx.BoxSizer(wx.HORIZONTAL)
        sizer_2.Add(self.input_text, 0, wx.ALIGN_CENTER_HORIZONTAL|wx.ALIGN_CENTER_VERTICAL, 0)
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
        self.data[row][col] = value


class SimpleGridForVM(wx.grid.Grid):
    def __init__(self, parent, data_list):
        wx.grid.Grid.__init__(self, parent, -1)
        getRowLabelsForVM()
        self.SetTableBase()
    
    def SetTableBase(self):
        self.tableBase = GenericTable(vm_data, rowLabelsForVM, colLabels)
        self.SetTable(self.tableBase)

    def UpdateValues( self ):
        """Update all displayed values"""
        msg = wx.grid.GridTableMessage(self.tableBase, wx.grid.GRIDTABLE_REQUEST_VIEW_GET_VALUES)
        self.ProcessTableMessage(msg)
        wx.grid.Grid.ForceRefresh(self) 


class SimpleGrid(wx.grid.Grid):
    def __init__(self, parent, data_list):
        wx.grid.Grid.__init__(self, parent, -1)
        getRowLabels()
        self.SetTableBase()
    
    def SetTableBase(self):
        self.tableBase = GenericTable(data, rowLabels, colLabels)
        self.SetTable(self.tableBase)

    def UpdateValues( self ):
        """Update all displayed values"""
        msg = wx.grid.GridTableMessage(self.tableBase, wx.grid.GRIDTABLE_REQUEST_VIEW_GET_VALUES)
        self.ProcessTableMessage(msg)
        wx.grid.Grid.ForceRefresh(self) 

    def SetValue(self, row, col, value):
        if row > -1 and col > -1:
          self.SetCellValue(row, col, value)



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
    def __init__(self, parent, mode):
        self.mode = mode
        self.parent = parent
        wx.Frame.__init__(self, self.parent, -1, "Virtuali masina", size=(1366, 719))
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
        if self.mode == "Supervizorius":
            self.NextButton = wx.Button(self, -1, "&Next step")
            self.Bind(wx.EVT_BUTTON, self.DoNextStep, self.NextButton)

        self.grid_1 = SimpleGridForVM(self, vm_data)
        self.__set_properties()
        self.__do_layout()

    def OnQuit(self, event):
        self.Close()

    def DoNextStep(self, event):
        DoStep(self)

    def setRegistersOnLoad(self):
        self.IC_text.SetValue(str(rm.processor.registers["IC"]))
        self.R1_text.SetValue(str(rm.processor.registers["R1"]))
        self.R2_text.SetValue(str(rm.processor.registers["R2"]))
        self.SF_text.SetValue(str(rm.processor.registers["SF"].toString()))

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
        fgs = wx.FlexGridSizer(1, 9, 5, 5)
        fgs.AddMany([
          (self.R1),(self.R1_text, 0,0,0), #R1 registras
          (self.R2),(self.R2_text, 0),     #R2 registras
          (self.SF), (self.SF_text, 0),     #SF registras
          (self.IC), (self.IC_text, 0)
        ])
        if self.mode == "Supervizorius":
            fgs.Add(self.NextButton, 0, 0)
        sizer_1.Add(fgs, 0,wx.ALIGN_CENTER_VERTICAL, 0)
        sizer_1.Add(self.grid_1, 4, wx.ALIGN_CENTER_VERTICAL, 0)
        self.SetSizer(sizer_1)
        sizer_1.Fit(self)
        self.Layout()
        self.setRegistersOnLoad()

class TestFrame(wx.Frame):
    def __init__(self, parent):
        wx.Frame.__init__(self, parent, -1, "Reali masina", size=(1366, 719))
        manu = MenuBar(self)

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

    def OnClick(self, event):
        if(self.MODE_radio.GetStringSelection() == "Vartotojas"):
            DoExecute(self)
        self.vmframe = VMFrame(self, self.MODE_radio.GetStringSelection())
        app.SetTopWindow(self.vmframe)
        self.vmframe.Centre()
        self.vmframe.Show()

    def OnQuit(self, event):
        self.Close()
        event.Skip()

    def setRegistersOnLoad(self):
        self.IC_text.SetValue(str(rm.processor.registers["IC"]))
        self.PLR_text.SetValue(str(rm.processor.registers["PLR"]))
        self.PLBR_text.SetValue(str(rm.processor.registers["PLBR"]))
        self.CHST_text.SetValue(str(rm.processor.registers["CHST"]))
        self.R1_text.SetValue(str(rm.processor.registers["R1"]))
        self.R2_text.SetValue(str(rm.processor.registers["R2"]))
        self.SF_text.SetValue(str(rm.processor.registers["SF"].toString()))
        self.IOI_text.SetValue(str(rm.processor.registers["IOI"]))
        self.SI_text.SetValue(str(rm.processor.registers["SI"]))
        self.PI_text.SetValue(str(rm.processor.registers["PI"]))

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
        self.setRegistersOnLoad()
        
def getRowLabels():
    for i in range(BLOCKS):
        rowLabels.append("%s" % i)

def getRowLabelsForVM():
    for i in range(rm.pager.get_C()):
        rowLabelsForVM.append("%s" % i)
    for i in range(150):
        rowLabelsForVM.append("%s" % i)

def onCellChange(x, y):
    pass


def DoExecute(caller):
    if not rm.processor.execute():
        stdout(rm.processor.error_message)
#    del vm_data[:]
#    del data[:]
#    for i in range(BLOCKS):
#        row = []
#        for j in range(BLOCK_SIZE):
#            row.append(rm.real_memory[i, j])
#        data.append(row)
#    for i in range(rm.pager.get_C()):
#        row = []
#        for j in range(BLOCK_SIZE):
#            row.append(rm.virtual_memory_code[i, j])
#        vm_data.append(row)
#    for i in range(rm.pager.get_D()):
#        row = []
#        for j in range(BLOCK_SIZE):
#            row.append(rm.virtual_memory_data[i, j])
#        vm_data.append(row)
#    caller.grid_1.UpdateValues()
#    caller.setRegistersOnLoad()
    return True

def DoStep(caller):
    if not rm.processor.step():
        stdout(rm.processor.error_message)
#    del vm_data[:]
#    del data[:]
#    for i in range(BLOCKS):
#        row = []
#        for j in range(BLOCK_SIZE):
#            row.append(rm.real_memory[i, j])
#        data.append(row)
#    for i in range(rm.pager.get_C()):
#        row = []
#        for j in range(BLOCK_SIZE):
#            row.append(rm.virtual_memory_code[i, j])
#        vm_data.append(row)
#    for i in range(rm.pager.get_D()):
#        row = []
#        for j in range(BLOCK_SIZE):
#            row.append(rm.virtual_memory_data[i, j])
#        vm_data.append(row)
#    caller.grid_1.UpdateValues()
#    caller.parent.grid_1.UpdateValues()
#    caller.setRegistersOnLoad()
    return True

def stdin():
    stdin_dialog = StdInDialog(None)
    return stdin_dialog.std_input

def stdout(message):
    stdout_dialog = StdOutDialog(message)
    return True

data = []
vm_data = []
colLabels = ["0","1", "2","3","4","5","6","7","8","9","A","B","C","D","E","F"]
rowLabelsForVM = []
rowLabels = []
app = wx.App()
rm = RealMachine(onCellChange)

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

        frame = TestFrame(None)
        app.SetTopWindow(frame)
        frame.Centre()
        frame.Show()
    except Exception, detail:
        stdout_dialog = StdOutDialog(unicode(detail))
        return 1
    app.MainLoop()
