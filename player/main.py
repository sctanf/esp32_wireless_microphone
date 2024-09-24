import pyaudio
import socket
import threading
from PyInquirer import prompt
from time import sleep
#import numpy as np

# select the output audio device using pyinquirer
def get_output_audio_device_idx(p: pyaudio.PyAudio):
    host_api_list = []
    for i in range(p.get_host_api_count()):
        host_api_info = p.get_host_api_info_by_index(i)
        host_api_list.append(host_api_info['name'])
    audio_device_list = []
    for i in range(p.get_device_count()):
        device = p.get_device_info_by_index(i)
        # if device["maxOutputChannels"] > 0 and "cable" in device["name"].lower():
        if device["maxOutputChannels"] > 0:
            audio_device_list.append({"name": host_api_list[device["hostApi"]] + ": " + device["name"], "checked": True})
    questions = [
        {
            "type": "list",
            "name": "audio_device",
            "message": "Select the audio device to use (I recommend you use a virtual audio device!)",
            "choices": audio_device_list,
        }
    ]
    answers = prompt(questions)
    # select the user's preffered audio device
    output_device_idx = [
        i
        for i in range(p.get_device_count())
        if answers["audio_device"] == host_api_list[p.get_device_info_by_index(i)["hostApi"]] + ": " + p.get_device_info_by_index(i)["name"]
    ][0]
    return output_device_idx

# select the input audio device using pyinquirer
def get_input_audio_device_idx(p: pyaudio.PyAudio):
    host_api_list = []
    for i in range(p.get_host_api_count()):
        host_api_info = p.get_host_api_info_by_index(i)
        host_api_list.append(host_api_info['name'])
    audio_device_list = []
    for i in range(p.get_device_count()):
        device = p.get_device_info_by_index(i)
        if device["maxInputChannels"] > 0 and "capture" in device["name"].lower():
            audio_device_list.append({"name": host_api_list[device["hostApi"]] + ": " + device["name"], "checked": True})
    questions = [
        {
            "type": "list",
            "name": "audio_device",
            "message": "Select the audio device to use (I recommend you use a virtual audio device!)",
            "choices": audio_device_list,
        }
    ]
    answers = prompt(questions)
    # select the user's preferred audio device
    input_device_idx = [
        i
        for i in range(p.get_device_count())
        if answers["audio_device"] == host_api_list[p.get_device_info_by_index(i)["hostApi"]] + ": " + p.get_device_info_by_index(i)["name"]
    ][0]
    return input_device_idx

# this will run in a thread reading audio from the tcp socket and buffering it
buffer = []
buffering = False
sock = socket.socket()

def connect():
    global sock
    sock = socket.socket()
    sock.settimeout(1)
    while True:
        try:
            sock.connect(("microphone1.local", 9090))
            break
        except:
            sleep(0.1)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    sock.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)

def read_audio_from_socket():
    global buffering, buffer, sock
    # connect to the esp32 socket
    connect()
    meanlist = []
    while True:
        try:
            data = sock.recv(128)
            if not data:
                print("Connection lost, reconnecting...")
                sock.close()
                connect()
            else:
#                # Convert the byte data to numpy array for processing
#                audio_data = np.frombuffer(data, dtype=np.int16)
#                # Remove DC offset
#                mean = np.mean(audio_data)
#                meanlist.append(mean)
#                if len(meanlist) > 1000:
#                    meanlist.pop(0)
#                mean = np.mean(meanlist)
#                audio_data = audio_data - mean
#                # Convert back to bytes
#                data = audio_data.astype(np.int16).tobytes()
                buffer.append(data)
        except socket.timeout:
            print("Socket timeout, reconnecting...")
            connect()
        except Exception as e:
            print(f"Socket error: {e}, reconnecting...")
            connect()

def stream_audio_to_socket(stream):
    global sock
    while True:
        try:
            data = stream.read(128, exception_on_overflow=False)
            sock.sendall(data)
        except socket.timeout:
            continue
        except Exception as e:
            continue

def main():
    global buffer, buffering
    # initialize pyaudio
    p = pyaudio.PyAudio()
    # kick off the audio buffering thread
    thread = threading.Thread(target=read_audio_from_socket)
    thread.daemon = True
    thread.start()
    output_device_idx = get_output_audio_device_idx(p)
    input_device_idx = get_input_audio_device_idx(p)
    # set up the audio in stream (from esp32)
    stream = p.open(
        output=True,
        rate=44100,
        channels=1,
        format=pyaudio.paInt16,
        output_device_index=output_device_idx,
        frames_per_buffer=256
    )
    # set up the audio out stream (to esp32)
    stream_out = p.open(
        format=pyaudio.paInt16,
        channels=2,
        rate=44100,
        input=True,
        input_device_index=input_device_idx,
        frames_per_buffer=512
    )
    thread_out = threading.Thread(target=stream_audio_to_socket, args=(stream_out,))
    thread_out.daemon = True
#    thread_out.start()
    # clear buffer
    buffer.clear()
    # this will write the buffered audio to the audio stream
    while True:
        while len(buffer) > 0:
            # under 100 there is too much underflow
            while len(buffer) > 100:
                # overflow
                buffer.pop(0)
            stream.write(buffer.pop(0))
        else:
            sleep(0.001)

if __name__ == "__main__":
    main()
