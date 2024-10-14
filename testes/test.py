import subprocess
import threading
import pexpect
import time
import pytest
import logging

# Configuração do logging para facilitar a depuração
logging.basicConfig(level=logging.DEBUG, format='%(threadName)s: %(message)s')

# Variável global protegida por um lock para sincronização entre threads
successful_connection = False
connection_lock = threading.Lock()

def init_service():
    global successful_connection
    logging.debug("Iniciando serviço...")
    try:
        # Inicia o serviço usando pexpect
        child = pexpect.spawn('python3 ../Service/Controller.py')
        # Define um timeout para evitar bloqueios indefinidos
        child.expect("Interface do Serviço>".encode("utf-8"), timeout=10)
        logging.debug("Prompt do serviço encontrado.")
        
        with connection_lock:
            successful_connection = True
        logging.debug(f"Conexão bem-sucedida: {successful_connection}")
        
    except pexpect.TIMEOUT:
        logging.error("Timeout ao esperar pelo prompt do serviço.")
    except pexpect.EOF:
        logging.error("EOF recebido antes de encontrar o prompt do serviço.")
    except Exception as e:
        logging.error(f"Erro inesperado na thread do serviço: {e}")

def init_agent():
    try:
        logging.debug("Inicializando agente...")
        # Executa o comando 'pwd' para depuração
        result = subprocess.run(['pwd'], capture_output=True, text=True, check=True)
        logging.debug(f"Diretório atual: {result.stdout.strip()}")
        
        # Inicia o agente
        subprocess.run(['python3', '../Agent/Controller.py'], check=True)
        logging.debug("Agente finalizado com sucesso.")
        
    except subprocess.CalledProcessError as e:
        logging.error(f"Erro ao iniciar agente: {e}")
    except Exception as e:
        logging.error(f"Erro inesperado na thread do agente: {e}")

@pytest.fixture(scope="module")
def setup_connection():
    # Cria as threads para o serviço e o agente
    service_thread = threading.Thread(target=init_service, name="ServiceThread")
    agent_thread = threading.Thread(target=init_agent, name="AgentThread")
    
    # Inicia a thread do serviço
    service_thread.start()
    # Aguarda um pouco para garantir que o serviço esteja pronto
    time.sleep(2)
    # Inicia a thread do agente
    agent_thread.start()
    
    yield  # Executa os testes
    
    # Aguarda ambas as threads terminarem após os testes
    service_thread.join()
    agent_thread.join()

def test_successful_connection(setup_connection):
    with connection_lock:
        assert successful_connection, "Conexão com o serviço não foi bem-sucedida."

