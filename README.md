# Código ERAD-NO2

Código desenvolvido para uso no minicurso apresentado durante o ERAD-NO2 2023.

Para compilar, você precisa do pacote [PMDK](https://github.com/pmem/pmdk) instalado em sua máquina. Utilize o Makefile fornecido.

Este repositório foi criado pelos professores:

- Alexandro Baldassin, Unesp
- Emilio Francesquini, UFABC

Este material está licenciado pela [CC-BY-SA 4.0](https://creativecommons.org/licenses/by-sa/4.0/deed.pt_BR).

## Smoke test

```
git clone git@github.com:baldas/minicurso-memoria-persistente.git
…
$ cd minicurso-memoria-persistente/
$ git checkout eradno2-2023
$ ./run_image.sh
...
erad@docker-pmem:~$ cd pmdk/pmkv/
erad@docker-pmem:pmkv$ make
…
erad@docker-pmem:~$ ./pmemkv
key: key1 value: value1
key: key3 value: value3
key: key2 value: value2
erad@docker-pmem:pmkv$
```
