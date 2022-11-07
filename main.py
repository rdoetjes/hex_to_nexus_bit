#!/usr/bin/python3
import rflib
import time
import sys
import bitstring

def get_number_of_zeros(pulse_us, bit_us):
    result = ""
    
    #prevent division by zero
    if pulse_us == 0:
        return result

    for i in range(0, int(bit_us/pulse_us) ):
        result += "0"
    return result

def char_to_int(chr):
    b = 0
    c = ord(chr)
    if c > 96 and c < 103: 
        b = c - 87 
    if c > 47 and c < 58: 
        b = c ^ 48
    return b

def hex_to_transmit_bitstring(hex, pulse_us, start_us, low_us, high_us):
    high_bit_sequence = "1"+get_number_of_zeros(pulse_us, high_us)
    low_bit_sequence = "1"+get_number_of_zeros(pulse_us, low_us)
    start_bit_sequence = "1"+get_number_of_zeros(pulse_us, start_us)

    #first we make a bit string, ideal for trouble shooting, than we'll convert that to hex
    result = start_bit_sequence    
    for chr in hex:
        if chr == " ":
            continue

        bits = format(char_to_int(chr), '04b')
        for bit in bits:
            if bit == "1": 
                result += high_bit_sequence
            if bit == "0": 
                result += low_bit_sequence
    return result

def binstr_to_bytestr(bin):
    bytes=[]
    i = 7
    c = 0
    for bit in bin:
        if bit == "1":
            c += 2 ** i
        i-=1
        if i == -1:
            bytes.append(c)
            i = 7
            c = 0

    return bytes

if __name__ == "__main__":
    PULSE_US = 500
    PULSE_START = 4000
    PULSE_LOW = 1000
    PULSE_HIGH = 2000

    # d = rflib.RfCat()

    # # Set Modulation. We using On-Off Keying here
    # d.setMdmModulation(rflib.MOD_ASK_OOK)
    # d.setMaxPower()
    # d.makePktFLEN(12)    # Set the RFData packet length
    # d.setMdmDRate(1/PULSE_US)  # Set the Baud Rate
    # d.setMdmSyncMode(0)  # Disable preamble
    # d.setFreq(433920000) # Set the frequency

    # 9b is the id (155)
    # 8 is batter okay 00 is battery empty
    # 068 is the temperature (12 bits the result is divided by 10 by the weather station to get the decimal)
    # f is a control nibble
    # 4 8 is the himidity (48%)
    if len(sys.argv) > 1:
        xmit =  hex_to_transmit_bitstring(sys.argv[1], PULSE_US, PULSE_START, PULSE_LOW, PULSE_HIGH)
    else:
        xmit = hex_to_transmit_bitstring("9b 80 68 f4 8", PULSE_US, PULSE_START, PULSE_LOW, PULSE_HIGH)

    #t = bitstring.BitArray(bin=xmit).tobytes()
    print(xmit)
    bytes = binstr_to_bytestr(xmit)
    print(bytes)

    for i in range(0,10):
        #d.RFxmit(bytes, repeat=10)
        print(xmit)
        time.sleep(10)

    #d.setModeIDLE()
