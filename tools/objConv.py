import ipywidgets as widgets
import struct
from tools import gui_fname
import os
from IPython.display import display
class objConvert:
    
    def __init__(self):
        self.fileName =''
        self.folder ='./'
        self.openButton = widgets.Button(
            value=False,
            description='Open obj',
            disabled=False,
            button_style='', # 'success', 'info', 'warning', 'danger' or ''
            tooltip='open obj file',
            icon='check'
        )
        self.uvSizeInput = widgets.BoundedIntText(
            value=128,
            min=0,
            max=1024,
            step=1,
            description='uv size:',
            disabled=False
        )
        self.outputSelect = widgets.RadioButtons(
            options=['Seperated array', 'Byte array'],
            value='Seperated array',
            description='Output type:',
            disabled=False
        )
        self.processButton = widgets.Button(
            value=False,
            description='Process',
            disabled=False,
            button_style='', # 'success', 'info', 'warning', 'danger' or ''
            tooltip='Process',
            icon='check'
        )
        self.placeHolder = widgets.HBox([self.openButton,self.uvSizeInput])
        self.placeHolder2 = widgets.HBox([self.processButton,self.outputSelect])
        self.mainPlaceHolder =  widgets.VBox([self.placeHolder,self.placeHolder2])
        self.processButton.on_click(self.ProcessObj)
        self.openButton.on_click(self.GetFileName)
        display(self.mainPlaceHolder)

    def ProcessObj(self,b):
        if self.fileName =='' or  self.fileName ==b'' :
            print('No obj file.')
            return
        self.outputType = self.outputSelect.value
        self.uvSize = self.uvSizeInput.value
        self.ReadInObj()
        if self.outputType == 'Seperated array':
            self.GenerateSeperate()
        else:
            self.GenerateByteArray()
    def GetFileName(self,b):
        self.fileName = gui_fname(directory=self.folder,fileType='Obj file (*.obj)')
        self.fileName = self.fileName.decode("utf-8") 
        self.folder = os.path.dirname(self.fileName)+'/'
        #self.folder=str(self.folder)[2:-1]
        
    def ReadInObj(self):
        self.v = []#output vector
        self.vn = []#
        self.vt =[]
        self.fvIdx=[]#output face index
        self.fvtIdx=[]
        self.fvnIdx=[]
        self.uvOutput=[]#uv output directly
        self.normalOutput =[]#normal output directly
        with open(self.fileName,"r") as file:
            objDatalines = file.readlines()
        for line in objDatalines:
            line =line.strip()
            if (line[0]=="#"):
                continue
            line = line.split(" ")


            for element in line:
                if element =='':
                    line.remove(element)


            if line[0] == "v" : #a vect line
                #print(line[1])
                self.v.append(float(line[1]))
                self.v.append(float(line[2]))
                self.v.append(float(line[3]))
            if line[0] == "vn" :#normal vect for face
                #print(line[1])
                self.vn.append(float(line[1]))
                self.vn.append(float(line[2]))
                self.vn.append(float(line[3]))
            if line[0] == "vt":
                #print(line[1])
                mapu = int(round(float(line[1])*(self.uvSize-1)))
                mapv = int(round(float(line[2])*(self.uvSize-1)))
                if mapu<0:
                    mapu=0
                if mapu>=self.uvSize:
                    mapu=self.uvSize -1
                if mapv<0:
                    mapv=0
                if mapv>=self.uvSize:
                    mapv=self.uvSize -1    

                self.vt.append(mapu)
                self.vt.append(mapv)
        for line in objDatalines:
            line = line[:-1]
            line = line.split(" ")
            for element in line:
                if element =='':
                    line.remove(element)

            if line[0] == "f":

                #print(line)
                #v/vt/vn
                faceData0 = line[1].split("/")

                self.fvIdx.append(int(faceData0[0])-1)
                self.fvtIdx.append(int(faceData0[1])-1)
                self.fvnIdx.append(int(faceData0[2])-1)
                faceData1 = line[2].split("/")
                self.fvIdx.append(int(faceData1[0])-1)
                self.fvtIdx.append(int(faceData1[1])-1)
                self.fvnIdx.append(int(faceData1[2])-1)
                faceData2 = line[3].split("/")
                self.fvIdx.append(int(faceData2[0])-1)
                self.fvtIdx.append(int(faceData2[1])-1)
                self.fvnIdx.append(int(faceData2[2])-1)
        #create output array
        for uvIdx in self.fvtIdx:
            mapu = self.vt[(uvIdx)*2]
            mapv = self.vt[(uvIdx)*2+1]
            self.uvOutput.append(mapu)
            self.uvOutput.append(mapv)

        if (self.fvnIdx[0]!=self.fvnIdx[1]):
            print("Not face normal!\n")
        for i in range(len(self.fvnIdx)//3):
            self.normalOutput.append(self.vn[(self.fvnIdx[i*3])*3])
            self.normalOutput.append(self.vn[(self.fvnIdx[i*3])*3+1])
            self.normalOutput.append(self.vn[(self.fvnIdx[i*3])*3+2])
        #cal the bound box
        vx=[]
        vy=[]
        vz=[]
        for i in range(len(self.v)//3):
            vx.append(self.v[3*i])
            vy.append(self.v[3*i+1])
            vz.append(self.v[3*i+2])
        vx.sort()
        vy.sort()
        vz.sort()
        self.vxMin=vx[0]
        self.vxMax=vx[-1]
        self.vyMin = vy[0]
        self.vyMax= vy[-1]
        self.vzMin= vz[0]
        self.vzMax= vz[-1]
        self.bound = []
        self.bound.append(self.vxMax)
        self.bound.append(self.vyMax)
        self.bound.append(self.vzMax)
        self.bound.append(self.vxMin)
        self.bound.append(self.vyMin)
        self.bound.append(self.vzMin)
        
    def GenerateSeperate(self):
        nameId =self.fileName.split("/")[-1][:-4]
        with open(self.fileName[:-3]+"c",'w') as outFile:
            with open(self.fileName[:-3]+"h",'w') as outHFile:
                outHFile.write("#ifndef __"+nameId+"_H__\n")
                outHFile.write("#define __"+nameId+"_H__\n")
                outFile.write("#include \"b3dlib.h\"\n")
                outFile.write("const float "+nameId+"_vect["+str(len(self.v))+"]={\n")
                outHFile.write("extern const float "+nameId+"_vect["+str(len(self.v))+"];\n")
                rowNum = int(len(self.v)/6)
                rowRest = len(self.v)%6
                for i in range(rowNum):
                    outputline = "\t"
                    for j in range(6):
                        outputline += (str(self.v[6*i+j])+'f, ')
                    outputline +='\n'
                    outFile.write(outputline)
                if rowRest !=0:
                    outputline="\t"
                    for i in range(rowRest):
                        outputline +=(str(self.v[6*rowNum+i])+'f, ')
                    outputline +='\n'
                    outFile.write(outputline)
                outFile.write("};\n")
                outFile.write("const float "+nameId+"_normal["+str(len(self.normalOutput))+"]={\n")
                outHFile.write("extern const float "+nameId+"_normal["+str(len(self.normalOutput))+"];\n")
                rowNum = int(len(self.normalOutput)/6)
                rowRest = len(self.normalOutput)%6
                for i in range(rowNum):
                    outputline = "\t"
                    for j in range(6):
                        outputline += (str(self.normalOutput[6*i+j])+'f, ')
                    outputline +='\n'
                    outFile.write(outputline)
                if rowRest !=0:
                    outputline="\t"
                    for i in range(rowRest):
                        outputline +=(str(self.normalOutput[6*rowNum+i])+'f, ')
                    outputline +='\n'
                    outFile.write(outputline)
                outFile.write("};\n")
                outFile.write("const uint16_t "+nameId+"_triIdx["+str(len(self.fvIdx))+"]={\n")
                outHFile.write("extern const uint16_t "+nameId+"_triIdx["+str(len(self.fvIdx))+"];\n")
                rowNum = int(len(self.fvIdx)/9)
                rowRest = len(self.fvIdx)%9
                for i in range(rowNum):
                    outputline = "\t"
                    for j in range(9):
                        outputline += (str(self.fvIdx[9*i+j])+', ')
                    outputline +='\n'
                    outFile.write(outputline)
                if rowRest !=0:
                    outputline="\t"
                    for i in range(rowRest):
                        outputline +=(str(self.fvIdx[9*rowNum+i])+', ')
                    outputline +='\n'
                    outFile.write(outputline)
                outFile.write("};\n")
                outFile.write("const uint8_t "+nameId+"_uv["+str(len(self.uvOutput))+"]={\n")
                outHFile.write("extern const uint8_t "+nameId+"_uv["+str(len(self.uvOutput))+"];\n")
                rowNum = int(len(self.uvOutput)/8)
                rowRest = len(self.uvOutput)%8
                for i in range(rowNum):
                    outputline = "\t"
                    for j in range(8):
                        outputline += (str(self.uvOutput[8*i+j])+', ')
                    outputline +='\n'
                    outFile.write(outputline)
                if rowRest !=0:
                    outputline="\t"
                    for i in range(rowRest):
                        outputline +=(str(self.uvOutput[8*rowNum+i])+', ')
                    outputline +='\n'
                    outFile.write(outputline)
                outFile.write("};\n")
                outFile.write("const B3L_Mesh_t "+nameId+"_mesh={\n")
                outFile.write("\t.vectNum = "+str(len(self.v)//3)+",\n")
                outFile.write("\t.triNum = "+str(len(self.fvIdx)//3)+",\n")
                outFile.write("\t.pVect = (float *)"+nameId+"_vect,\n")
                outFile.write("\t.pTri = (uint16_t *)"+nameId+"_triIdx,\n")
                outFile.write("\t.pUv = (uint8_t *)"+nameId+"_uv,\n")
                outFile.write("\t.pNormal = (float *)"+nameId+"_normal,\n")
                outFile.write("};\n")
                outHFile.write("extern const B3L_Mesh_t "+nameId+"_mesh;\n")

                #reference point is in the middle of the model
                
                outFile.write("const float "+nameId+"bound[6]={\n")
                outFile.write("\t"+str(self.vxMax)+"f, "+str(self.vyMax)+"f, "+str(self.vzMax)+"f,\n")
                outFile.write("\t"+str(self.vxMin)+"f, "+str(self.vyMin)+"f, "+str(self.vzMin)+"f,\n")
                outFile.write("};\n")
                outHFile.write("extern const float "+nameId+"bound[6];\n")
                outHFile.write("#endif \n")

    def GenerateByteArray(self):
        vectNum = int(len(self.v)/3)
        triNum = int(len(self.fvIdx)/3)
        wholeData = struct.pack("I",0)#0 is texture mesh type
        wholeData +=struct.pack("H",vectNum)
        wholeData +=struct.pack("H",triNum)
        for element in self.v:
            wholeData +=struct.pack("f",element)
        for element in self.normalOutput:
            wholeData +=struct.pack("f",element)    
        for element in self.bound:
            wholeData +=struct.pack("f",element)
        for element in self.fvIdx:
            wholeData +=struct.pack("H",element)
        for element in self.uvOutput:
            wholeData +=struct.pack("B",element)
        with open(self.fileName[:-3]+"bin",'w+b') as outFile:
            outFile.write(bytearray(wholeData))
