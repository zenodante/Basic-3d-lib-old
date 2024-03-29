
# coding: utf-8

# In[ ]:



from sys import executable, argv
from subprocess import check_output
from PyQt5.QtWidgets import QFileDialog, QApplication

def gui_openfolder(directory='./'):
    """Open a file dialog, starting in the given directory, and return
    the chosen filename"""
    # run this exact file in a separate process, and grab the result
    file = check_output([executable, __file__, directory])
    return file.strip()

if __name__ == "__main__":
    directory = argv[1]
    app = QApplication([directory])
    folder = QFileDialog.getExistingDirectory(None, "Select a folder...", 
            directory)
    print(folder)

