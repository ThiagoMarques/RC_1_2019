# Inspetor HTTP baseado em Proxy Server

Sistema de inspeção e análise de requisições HTTP através de um servidor proxy desenvolvido durante a graduação na Universidade de Brasília.

## Sobre

Este projeto implementa um inspetor HTTP baseado em servidor proxy, permitindo interceptar, analisar e inspecionar requisições e respostas HTTP em tempo real.

## Funcionalidades

- Interceptação de requisições HTTP
- Análise de headers e body
- Logging de requisições e respostas
- Inspeção de tráfego de rede
- Proxy server funcional

## Como Usar

### Pré-requisitos
- Node.js (versão recomendada: 14+)
- npm ou yarn

### Instalação

```bash
# Clonar o repositório
git clone https://github.com/ThiagoMarques/RC_1_2019.git

# Instalar dependências
npm install
```

### Execução

```bash
# Iniciar o servidor proxy
npm start

# Ou com Node.js diretamente
node server.js
```

### Configuração

Configure seu navegador ou aplicação para usar o proxy:
- **Host**: localhost
- **Porta**: (verificar no código/configuração)

## Estrutura do Projeto

```
RC_1_2019/
├── server.js          # Servidor proxy principal
├── package.json       # Dependências do projeto
├── README.md          # Este arquivo
└── ...
```

## Tecnologias Utilizadas

- Node.js
- HTTP/HTTPS modules
- Outras dependências (verificar package.json)

## Notas

Este projeto foi desenvolvido para fins educacionais e de análise de tráfego HTTP.

## Aviso

Este tipo de ferramenta deve ser usado apenas para fins legítimos:
- Desenvolvimento e debugging
- Análise de segurança autorizada
- Testes em ambientes controlados

## Contribuindo

Contribuições são bem-vindas! Sinta-se à vontade para:
- Reportar bugs
- Sugerir melhorias
- Enviar pull requests

## Licença

Este projeto é de código aberto e está disponível para fins educacionais.

---

**Autor**: Thiago Marques  
**Ano**: 2019
