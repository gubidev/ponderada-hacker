# Relatório Técnico de Análise de Riscos: ESP32 Web Server

Este documento consolida a análise de segurança estática do código-fonte do servidor web do ESP32, identificando vulnerabilidades críticas, detalhando a avaliação de risco dos ataques e propondo medidas de mitigação.

---

## 1.Resumo das Vulnerabilidades Críticas

O servidor web atual é **altamente vulnerável**, pois **não implementa nenhuma autenticação ou criptografia**, e as credenciais de rede estão expostas.

| ID | Vulnerabilidade | Descrição |
| :--- | :--- | :--- |
| 01 | **Controle Não Autorizado** | Qualquer cliente na rede pode enviar comandos (ex: `/26/on`) e controlar o hardware, sem verificação de identidade (autenticação). |
| 02 | **Comunicação em Texto Simples** | O uso de HTTP (porta 80) expõe todos os comandos e respostas a ataques de *sniffing* na rede. |
| 03 | **Exposição de Credenciais no Código-Fonte** | As credenciais de Wi-Fi (`ssid` e `password`) estão codificadas diretamente, tornando-as visíveis a qualquer pessoa com acesso ao código ou firmware. |
| 04 | **Falta de Limitação de Taxa** | O código não possui mecanismos para limitar o número de requisições por cliente, facilitando ataques de Negação de Serviço (DoS). |

---

## 2. Análise Detalhada dos Ataques e Risco

Abaixo estão os ataques mais críticos identificados, com suas avaliações de risco detalhadas (Probabilidade, Impacto e Risco).

### Ataque 1: Comprometimento da Rede Wi-Fi (Risco: CRÍTICO)

Explora a vulnerabilidade **V-03 (Exposição de Credenciais)**.

* **Passo-a-Passo:**
    1.  O atacante obtém acesso ao **código-fonte** do projeto.
    2.  O atacante extrai o **Password do Wi-Fi** diretamente das constantes globais (`const char* password = ...`).
    3.  O atacante usa a senha roubada para se conectar à **rede Wi-Fi principal**, obtendo acesso a todos os outros dispositivos conectados.
* **Probabilidade:** **Média/Alta**. *Justificativa:* O código-fonte é frequentemente compartilhado (ex: Git, repositório interno). A credencial está em texto claro no código.
* **Impacto:** **Crítico**. *Justificativa:* O ataque compromete **toda a rede**, não apenas o ESP32, possibilitando roubo de dados e acesso a sistemas críticos.

### Ataque 2: Comando Remoto e Sabotagem (Risco: ALTO)

Explora a vulnerabilidade **V-01 (Falta de Autenticação)**.

* **Passo-a-Passo:**
    1.  O atacante se conecta à mesma **rede Wi-Fi local**.
    2.  Usa ferramentas de varredura (ex: Nmap) para descobrir o **endereço IP** do ESP32.
    3.  Envia requisições HTTP diretas (ex: `http://<IP_do_ESP32>/26/on`) para controlar os GPIOs.
* **Probabilidade:** **Muito Alta**. *Justificativa:* O ataque é trivial; qualquer pessoa na rede pode fazê-lo sem autenticação ou ferramentas avançadas.
* **Impacto:** **Alto**. *Justificativa:* Pode causar danos físicos (se o GPIO controlar um relé ou motor) ou operacionais, dependendo do equipamento conectado.

---

## 3.Tabela Consolidada de Riscos (Decrescente)

Os ataques são ordenados do **maior risco** para o **menor risco**.

| Título do Ataque | Vulnerabilidade Explorada | Probabilidade | Impacto | Risco |
| :--- | :--- | :--- | :--- | :--- |
| **Comprometimento da Rede Wi-Fi** | V-03 (Exposição de Senha) | Média/Alta | Crítico | **CRÍTICO** |
| **Comando Remoto e Sabotagem** | V-01 (Falta de Autenticação) | Muito Alta | Alto | **ALTO** |
| Sniffing (Escuta Passiva) | V-02 (HTTP Não Criptografado) | Média | Médio | **MÉDIO** |

---

## 4.Mitigação e Recomendações

As seguintes ações são críticas para garantir a segurança da solução IoT:

### Ação 1: Proteger Credenciais 

* **Correção:** As credenciais de Wi-Fi (`ssid` e `password`) devem ser lidas de um local seguro, como a **memória flash (Preferences/EEPROM)**, e **NÃO** codificadas no código-fonte. Isso impede que a senha da rede seja distribuída junto com o código.

### Ação 2: Implementar Autenticação 

É obrigatório restringir o acesso apenas a clientes autorizados.

* **Método Recomendado:** Usar um **Token de Acesso Secreto** na URL. O código deve verificar se a string de solicitação contém o token correto (ex: `header.indexOf("token=CHAVE_SECRETA_UNICA")`).
* **Melhoria Adicional:** Implementar a **Autenticação HTTP Digest** (mais segura que a HTTP Basic) para acesso à página principal.

### Ação 3: Criptografar a Comunicação 

O tráfego de comandos deve ser criptografado para proteger contra *sniffing* e ataques Man-in-the-Middle (MITM).

* **Correção:** Migrar de **HTTP** para **HTTPS (SSL/TLS)**.
    * Requer o uso da biblioteca **`WiFiClientSecure.h`** e um certificado digital.
    * Altere o servidor para escutar na porta **443** (padrão HTTPS): `WiFiServer server(443);`.

### Ação 4: Limitação de Taxa (Mitiga V-04)

* **Correção:** Adicionar um mecanismo simples para rastrear o endereço IP do cliente e **bloquear temporariamente** o acesso se ele fizer muitas requisições em um curto período de tempo (prevenção de **Denial of Service - DoS**).
