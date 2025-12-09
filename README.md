## 1. Resumo das Vulnerabilidades Críticas

O servidor web atual é altamente vulnerável, pois **não implementa nenhuma autenticação ou criptografia**. Qualquer dispositivo na mesma rede Wi-Fi pode enviar comandos para controlar os GPIOs.

| ID | Vulnerabilidade | Descrição |
| :--- | :--- | :--- |
| V-01 | **Controle Não Autorizado** | Qualquer cliente pode enviar comandos (ex: `/26/on`) e controlar o hardware. |
| V-02 | **Comunicação em Texto Simples** | O uso de HTTP expõe todos os comandos e respostas a ataques de *sniffing* na rede. |

---

## 2. Mitigação e Recomendações (Como Corrigir)

As seguintes ações são críticas para garantir a segurança da solução IoT:

### Ação 1: Implementar Autenticação (V-01)

É obrigatório restringir o acesso apenas a clientes autorizados.

* **Método Recomendado:** Usar um **Token de Acesso Secreto** na URL. O código deve verificar se a string de solicitação contém o token correto (ex: `header.indexOf("token=SEGREDO123")`).
* **Melhoria Adicional:** Implementar a **Autenticação HTTP Digest** (mais segura que a HTTP Basic) para acesso à página principal.

### Ação 2: Criptografar a Comunicação (V-02)

O tráfego de comandos deve ser criptografado para proteger contra *sniffing* e ataques Man-in-the-Middle (MITM).

* **Correção:** Migrar de **HTTP** para **HTTPS (SSL/TLS)**.
    * Requer o uso da biblioteca **`WiFiClientSecure.h`** e um certificado digital.
    * Altere o servidor para escutar na porta 443 (padrão HTTPS): `WiFiServer server(443);`.

### Ação 3: Limitar Exposição e Credenciais

* **Credenciais (A3):** As credenciais de Wi-Fi (`ssid` e `password`) devem ser lidas de um local seguro, como a **memória flash (EEPROM)**, e não codificadas no código-fonte.
* **Limitação de Taxa (A4):** Adicionar um mecanismo simples que rastreie o IP do cliente e **bloqueie temporariamente** o acesso se ele fizer muitas requisições por segundo (prevenção de Denial of Service - DoS).

---

## 3. Conclusão

A segurança do ESP32 Web Server é crítica, especialmente quando controla equipamentos físicos. A **falta de Autenticação** representa uma falha de "porta aberta" (risco **ALTO**) que deve ser abordada antes de qualquer implantação em um ambiente de produção ou doméstico.
