import subprocess
import threading
import pexpect
import time
from mutagen.wave import WAVE
from mutagen import MutagenError

successful_connection = False
help_message = False
start_monitoring = False
status = False
list_files = False
pull_audio = False
remove_file = False
rename_file = False

def file_is_wav(file_path):
        try:
            audio = WAVE(file_path)  
            return audio.info is not None
        except MutagenError as e:
            return False
        except FileNotFoundError:
            print("Arquivo não encontrado")
            return False

def init_service():
	child = pexpect.spawn('python3 Service/Controller.py')
	child.expect("Interface do Serviço>".encode("utf-8"))
	global successful_connection
	successful_connection = True
	
def init_agent():
	subprocess.run(['python3', 'Agent/Controller.py'])
	
	
def service_help_message():
	child = pexpect.spawn('python3 Service/Controller.py')
	child.expect("Interface do Serviço>".encode("utf-8"))
	child.sendline("help")
	child.expect("Interface do Serviço>".encode("utf-8"))
	
	EXPECTED_SUBSTRING = "Documented commands (type help <topic>):" 
	if EXPECTED_SUBSTRING in child.before.decode('utf-8'):
		global help_message
		help_message = True
		
def service_start_monitoring():
	child = pexpect.spawn('python3 Service/Controller.py')
	child.expect("Interface do Serviço>".encode("utf-8"))
	child.sendline("START_MONITORING 0 1 3")
	child.expect("Interface do Serviço>".encode("utf-8"))
	
def agent_start_monitoring():
	result = subprocess.run(['python3', 'Agent/Controller.py'], capture_output=True, text=True)
	
	if file_is_wav(result.stdout.split("\n")[0]):
		global start_monitoring
		start_monitoring = True
		
def service_status():
	child = pexpect.spawn('python3 Service/Controller.py')
	child.expect("Interface do Serviço>".encode("utf-8"))
	child.sendline("STATUS")
	child.expect("Interface do Serviço>".encode("utf-8"))
	
	if "Audio Recorder: audio is ready" in child.before.decode('utf-8') and "Data Transporter: ok" in child.before.decode('utf-8'):
		global status
		status = True
		
def service_list_files():
	child = pexpect.spawn('python3 Service/Controller.py')
	child.expect("Interface do Serviço>".encode("utf-8"))
	child.sendline("LIST")
	child.expect("Interface do Serviço>".encode("utf-8"))
	
	if "0 -" in child.before.decode('utf-8'):
		global list_files
		list_files = True	
		
def service_remove_file():
	child = pexpect.spawn('python3 Service/Controller.py')
	child.expect("Interface do Serviço>".encode("utf-8"))
	child.sendline("DELETE 0")
	child.expect("Interface do Serviço>".encode("utf-8"))
	
	if "file deleted: audio" in child.before.decode('utf-8'):
		global remove_file
		remove_file = True	
		
def service_rename_file():
	child = pexpect.spawn('python3 Service/Controller.py')
	child.expect("Interface do Serviço>".encode("utf-8"))
	child.sendline("RENAME 0 \"audioFile.wav\"")
	child.expect("Interface do Serviço>".encode("utf-8"))
	
	if "File audio" in child.before.decode('utf-8') and "renamed to: audioFile.wav" in child.before.decode('utf-8'):
		global rename_file
		rename_file = True	
				
def service_pull_audio():
	child = pexpect.spawn('python3 Service/Controller.py')
	child.expect("Interface do Serviço>".encode("utf-8"))
	child.sendline("PULL_AUDIO \"audioFile.wav\"")
	child.expect("Interface do Serviço>".encode("utf-8"))
	
	if file_is_wav("correspondencia/audioFile.wav"):
		global pull_audio
		pull_audio = True
	
def test_successful_connection():
	service_thread = threading.Thread(target=init_service)
	agent_thread = threading.Thread(target=init_agent)
	
	service_thread.start()
	time.sleep(2)
	agent_thread.start()
	
	agent_thread.join()
	service_thread.join()
	
	global successful_connection
	assert successful_connection
	
def test_help_message():
	service_thread = threading.Thread(target=service_help_message)
	agent_thread = threading.Thread(target=init_agent)
	
	service_thread.start()
	time.sleep(2)
	agent_thread.start()
	
	agent_thread.join()
	service_thread.join()
	
	global help_message
	assert help_message
	
def test_start_monitoring():
	service_thread = threading.Thread(target=service_start_monitoring)
	agent_thread = threading.Thread(target=agent_start_monitoring)
	
	service_thread.start()
	time.sleep(2)
	agent_thread.start()
	
	agent_thread.join()
	service_thread.join()
	
	global start_monitoring
	assert start_monitoring
	
def test_status():
	service_thread = threading.Thread(target=service_status)
	agent_thread = threading.Thread(target=init_agent)
	
	service_thread.start()
	time.sleep(2)
	agent_thread.start()
	
	agent_thread.join()
	service_thread.join()
	
	global status
	assert status

def test_list_audio_files():
	service_thread = threading.Thread(target=service_list_files)
	agent_thread = threading.Thread(target=init_agent)
	
	service_thread.start()
	time.sleep(2)
	agent_thread.start()
	
	agent_thread.join()
	service_thread.join()
	
	global list_files
	assert list_files

def test_remove_audio_file():
	service_thread = threading.Thread(target=service_remove_file)
	agent_thread = threading.Thread(target=init_agent)
	
	service_thread.start()
	time.sleep(2)
	agent_thread.start()
	
	agent_thread.join()
	service_thread.join()
	
	global remove_file
	assert remove_file

def test_rename_audio_file():
	service_thread = threading.Thread(target=service_rename_file)
	agent_thread = threading.Thread(target=init_agent)
	
	service_thread.start()
	time.sleep(2)
	agent_thread.start()
	
	agent_thread.join()
	service_thread.join()
	
	global rename_file
	assert rename_file

def test_pull_audio():
	service_thread = threading.Thread(target=service_pull_audio)
	agent_thread = threading.Thread(target=init_agent)
	
	service_thread.start()
	time.sleep(2)
	agent_thread.start()
	
	agent_thread.join()
	service_thread.join()
	
	global pull_audio
	assert pull_audio
		
	
test_successful_connection()
test_help_message()
test_start_monitoring()
test_status()
test_rename_audio_file()
test_pull_audio()
test_list_audio_files()
test_remove_audio_file()

