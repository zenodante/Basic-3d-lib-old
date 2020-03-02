
# coding: utf-8

# In[ ]:



from sys import executable, argv
from subprocess import check_output
from PyQt5.QtWidgets import QFileDialog, QApplication

def gui_savefname(directory='./',defaultName='output',fileType ='All files (*)'):
    """Open a file dialog, starting in the given directory, and return
    the chosen filename"""
    # run this exact file in a separate process, and grab the result
    file = check_output([executable, __file__, directory,defaultName,fileType])
    return file.strip()

if __name__ == "__main__":
    directory = argv[1]
    defaultName=argv[2]
    fileType=argv[3]
    app = QApplication([directory])
    fname = QFileDialog.getSaveFileName(None, "Create a file...", 
            (directory+defaultName), filter=fileType)
    print(fname[0])

