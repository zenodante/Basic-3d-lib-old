import ipywidgets as widgets
import struct
from tools import gui_fname
import os
from PIL import Image
from IPython.display import display
class imgConvert:
    def __init__(self):
        self.fileName =''
        self.folder ='./'
        self.openButton = widgets.Button(
            value=False,
            description='Open png',
            disabled=False,
            button_style='', # 'success', 'info', 'warning', 'danger' or ''
            tooltip='open obj file',
            icon='check'
        )
        self.transInput = widgets.BoundedIntText(
            value=0,
            min=0,
            max=255,
            step=1,
            description='Trans color:',
            disabled=False
        )
        self.colorTypeSelect = widgets.RadioButtons(
            options=['4 colors','16 colors', '256 colors'],
            value='16 colors',
            description='Output type:',
            disabled=False
        )
        self.outColorTypeSelect = widgets.RadioButtons(
            options=['ARGB8888', 'ARGB4444'],
            value='ARGB8888',
            description='Output LUT:',
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
        self.placeHolder = widgets.HBox([self.openButton,self.transInput])
        self.placeHolder2 = widgets.HBox([self.processButton,self.colorTypeSelect,self.outColorTypeSelect])
        self.mainPlaceHolder =  widgets.VBox([self.placeHolder,self.placeHolder2])
        display(self.mainPlaceHolder)
        self.processButton.on_click(self.ProcessPng)
        self.openButton.on_click(self.GetFileName)
    
    def ProcessPng(self,b):
        im = Image.open(self.fileName)
        x,y = im.size
        testPal = im.getpalette()  
        lut=[]
        imData = im.load()
        indexVal =[]
        if self.colorTypeSelect.value == '16 colors':
            colorNum = 16
            lutColorNumMarker = 1
        elif self.colorTypeSelect.value == '256 colors':
            colorNum = 256
            lutColorNumMarker = 2
        elif self.colorTypeSelect.value == '4 colors':
            colorNum = 4
            lutColorNumMarker = 0
        for i in range(colorNum):
            r = testPal[3*i]
            g = testPal[3*i+1]
            b = testPal[3*i+2]
            if self.outColorTypeSelect.value == 'ARGB8888':  
                temp = (0xFF<<24)|(r<<16)|(g<<8)|b    
            else:
                r = int(round(r/16.0))
                g = int(round(g/16.0))
                b = int(round(b/16.0))
                temp= (0xF<<12)|(r<<8)|(g<<4)|b
            lut.append(temp)    
                
        if self.colorTypeSelect.value == '16 colors':            
            for i in range(y):
                for j in range(x//2):
                    lowPixel = imData[j*2,y-1-i]
                    highPixel = imData[j*2+1,y-1-i]
                    #print(lowPixel)
                    #print(highPixel)
                    finalVal = (highPixel<<4)|(lowPixel)
                    indexVal.append(finalVal)
        elif self.colorTypeSelect.value == '256 colors':    
            for i in range(y):
                for j in range(x):
                    pixel = imData[j,y-1-i]
                    indexVal.append(pixel) 
        elif self.colorTypeSelect.value == '4 colors':  
            for i in range(y):
                for j in range(x//4):
                    pixel0=imData[j*4,y-1-i]
                    pixel1=imData[j*4+1,y-1-i]
                    pixel2=imData[j*4+2,y-1-i]
                    pixel3=imData[j*4+3,y-1-i]
                    finalVal = (pixel3<<6)|(pixel2<<4)|(pixel1<<2)|pixel0
                    indexVal.append(finalVal)
                    
        dataLength = len(indexVal)
        nameId =self.fileName.split("/")[-1][:-4]
        with open(self.fileName[:-4]+'_img.c','w') as outputFile: 
            with open(self.fileName[:-4]+'_img.h','w') as outputHFile:
                outputHFile.write("#ifndef __"+nameId+"_IMG_H__\n")
                outputHFile.write("#define __"+nameId+"_IMG_H__\n")
                outputHFile.write("#include \"b3dlib.h\"\n")
                outputFile.write("#include \"b3dlib.h\"\n")
                outputFile.write('//image size %d ,%d\n'%(x,y))
                outputHFile.write('//image size %d ,%d\n'%(x,y))
                outputFile.write('//image LUT color number %d \n'%(colorNum))
                outputHFile.write('//image LUT color number %d \n'%(colorNum))
                if self.outColorTypeSelect.value == 'ARGB8888':
                    outputFile.write('//image color lut type: ARGB8888 \n')
                    outputFile.write('const uint32_t '+ nameId +'_uvLut['+str(colorNum)+']={\n')
                    outputHFile.write('extern const uint32_t '+ nameId +'_uvLut['+str(colorNum)+'];\n')
                    for i in range(colorNum//4):
                        for j in range(4):
                            outputFile.write('%#08X, '%lut[i*4+j])
                        outputFile.write('\n') 
                    outputFile.write('};\n')   
                else:
                    outputFile.write('//image color lut type: ARGB4444 \n')
                    outputFile.write('const uint16_t '+ nameId +'_uvLut['+str(colorNum)+']={\n')
                    outputHFile.write('extern const uint16_t '+ nameId +'_uvLut['+str(colorNum)+'];\n')
                    for i in range(colorNum//4):
                        for j in range(4):
                            outputFile.write('%#04X, '%lut[i*4+j])
                        outputFile.write('\n')
                    outputFile.write('};\n')   

                outputFile.write('const uint8_t '+ nameId +'_uvImg['+str(dataLength)+']={\n')
                outputHFile.write('extern const uint8_t '+ nameId +'_uvImg['+str(dataLength)+'];\n')
                left = dataLength%16
                for i in range(int(dataLength/16)):
                    outputFile.write('\t')
                    for j in range(16):
                        outputFile.write('%#02X, '%indexVal[16*i+j])
                    outputFile.write('\n')
                if left != 0:
                    for k in range(left):
                        outputFile.write('\t')
                        outputFile.write('%#02X, '%indexVal[int(dataLength/16)+k])
                    outputFile.write('\n')    
                outputFile.write('};\n') 
                outputFile.write('B3L_texture_t '+nameId+'={\n')
                if self.colorTypeSelect.value == '16 colors':
                    temp = 'LUT16'
                elif self.colorTypeSelect.value == '256 colors':
                    temp = 'LUT256'
                elif self.colorTypeSelect.value == '4 colors':
                    temp = 'LUT4'
                outputFile.write('\t .type ='+temp +',\n')
                outputFile.write('\t .uvSize='+str(x)+',\n')
                if self.outColorTypeSelect.value == 'ARGB8888':
                    outputFile.write('\t .pLUT=(uint32_t *)'+ nameId +'_uvLut,\n')
                else:
                    outputFile.write('\t .pLUT=(uint16_t *)'+ nameId +'_uvLut,\n')
                outputFile.write('\t .pData=(uint8_t *)'+nameId +'_uvImg,\n')
                outputFile.write('\t .transColorIdx='+str(self.transInput.value)+'};\n')
                outputHFile.write('extern B3L_texture_t '+nameId+';\n')
                outputHFile.write("#endif \n")
        #generate bin file
        if self.outColorTypeSelect.value == 'ARGB8888':
            wholeData = struct.pack("H",0)#0 8888 lut
        else:
            wholeData = struct.pack("H",1)#1 is 4444 lut
        wholeData += struct.pack("H",lutColorNumMarker)
        wholeData += struct.pack("H",x)
        wholeData += struct.pack("H",y)
        for i in lut:
            if  self.outColorTypeSelect.value == 'ARGB8888':
                wholeData += struct.pack('I',i)
            else:
                wholeData += struct.pack('H',i)
        for i in indexVal:
            wholeData += struct.pack('B',i)
        with open(self.fileName[:-4]+"_img.bin",'w+b') as outFile:
            outFile.write(bytearray(wholeData))
        print(len(wholeData))
    def GetFileName(self,b):
        self.fileName = gui_fname(directory=self.folder,fileType='Obj file (*.png)')
        self.fileName = self.fileName.decode("utf-8") 
        self.folder = os.path.dirname(self.fileName)+'/'
    
 