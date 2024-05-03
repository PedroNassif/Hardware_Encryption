// -------------------------------Criptografia AES-128-CBC----------------
// -----------------------Pedro Miguel Nassif, março 2024 ----------------
// ------------------------Aplicação para um estudo acâdemico - TCC -----

#include <AESLib.h>


AES aes;
byte cipher[1000];
char b64[10000];

uint64_t noiseValueForKey = 0;
byte key[16];

// Função para criptografar a mensagem usando uma chave e IV fornecidos
void do_encrypt(String msg, byte key[16], String iv_str) {
  byte iv[16];

  // Copiar o vetor inicial (IV) para o conteúdo do array
  memcpy(iv, (byte *)iv_str.c_str(), 16);

  // Usar a base64 para criptografar o conteúdo da mensagem
  int blen = base64_encode(b64, (char *)msg.c_str(), msg.length());

  // Calcular o tamanho da saída
  aes.calc_size_n_pad(blen);

  // Preenchimento customizado, neste caso vai ser zero
  int len = aes.get_size();
  byte plain_p[len];
  for (int i = 0; i < blen; i++) {
    plain_p[i] = b64[i];
  }

  for (int i = blen; i < len; i++) {
    plain_p[i] = '\0';
  }

  // Realizar a criptografia AES-128-CBC
  int blocks = len / 16;
  aes.set_key(key, 16);
  aes.cbc_encrypt(plain_p, cipher, blocks, iv);

  // Usar a base64 para codificar os dados criptografados
  base64_encode(b64, (char *)cipher, len);
  Serial.println("Encrypted Data out: " + String((char *)b64));
}

// Função para descriptografar os dados criptografados e imprimir a mensagem original
void do_decrypt(byte key[16], String iv_str) {
  // Decodificar os dados de Base64 para obter os dados criptografados
  int len = base64_decode((char *)cipher, b64, strlen(b64));

  // Inicializar o IV
  byte iv[16];
  memcpy(iv, (byte *)iv_str.c_str(), 16);

  // Setar a chave
  aes.set_key(key, 16);

  // Descifrar os dados
  int blocks = len / 16;
  aes.cbc_decrypt(cipher, cipher, blocks, iv);

  // Remover o preenchimento (padding)
  int pad_len = cipher[len - 1];
  len -= pad_len;

  // Decodificar os dados de Base64 para obter a mensagem original
  char decoded_msg[len];
  base64_decode(decoded_msg, (char *)cipher, len);

  Serial.print("Decrypted Data: ");
  Serial.println(decoded_msg);
}

void setup() {
  Serial.begin(9600);
  Serial.println();
  Serial.println();

  // Combinar múltiplas leituras do ADC para aumentar a entropia
  for (int i = 0; i < 8; i++) {
  uint16_t adcValue = analogRead(4);
  uint32_t randomValue = random(0xFFFF); // Gerar um valor aleatório de 16 bits
  noiseValueForKey ^= (static_cast<uint64_t>(adcValue) << (i * 12)) ^ (static_cast<uint64_t>(randomValue) << (i * 16));
  delay(10); // Atraso entre leituras para aumentar a aleatoriedade
  }
  // Copiar os bytes do valor combinado para a chave
  memcpy(key, &noiseValueForKey, sizeof(noiseValueForKey));

  // Imprimir o valor combinado (para fins de depuração)
  Serial.print("Valor do ruído combinado: ");
  Serial.println(noiseValueForKey, HEX);
}

void loop() {
  Serial.println();
  String msg = "AES-128-CBC #teste de EnCrYpTaCãO do TCC de https://github.com/PedroNassif ";
  byte key[16]; // Chave AES-128
 // readNoiseForEncryption(key); // Ler ruído da porta analógica e formatar como chave

  uint64_t noiseValueForIV = ((1 + analogRead(4) * 739 * (analogRead(A0) ^ 7)) / 13) * 4739772123189;
  String iv_str = String(noiseValueForIV); // Vetor de inicialização (IV) fixo, por simplicidade
  do_encrypt(msg, key, iv_str);
  do_decrypt(key, iv_str);
  delay(5000);
}


