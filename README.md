# fetchmail — COMP30023 Project 2

A command-line IMAP email client written in C, supporting both plain (port 143) and TLS-encrypted (port 993) connections.

## Build

```bash
make
```

Requires OpenSSL (`libssl`, `libcrypto`) to be installed.

To clean build artifacts:

```bash
make clean
```

## Usage

```
./fetchmail -u <username> -p <password> [-f <folder>] [-n <message>] [-t] <command> <server>
```

### Options

| Flag | Description |
|------|-------------|
| `-u <username>` | IMAP account username (required) |
| `-p <password>` | IMAP account password (required) |
| `-f <folder>` | Mailbox folder to operate on (default: `INBOX`) |
| `-n <number>` | Message sequence number (required for `retrieve`, `parse`, `mime`) |
| `-t` | Use TLS/SSL on port 993 instead of plain IMAP on port 143 |

### Commands

| Command | Description |
|---------|-------------|
| `list` | List all messages in the specified folder |
| `retrieve` | Print the raw body of message `-n` |
| `parse` | Print parsed `From`, `To`, `Date`, and `Subject` headers of message `-n` |
| `mime` | Print the MIME structure of message `-n` |

## Source Files

| File | Description |
|------|-------------|
| `main.c` | Argument parsing and entry point |
| `socket.c/h` | TCP/TLS connection setup and IMAP command I/O |
| `retrieve.c/h` | `retrieve` command implementation |
| `parse.c/h` | `parse` command — email header extraction |
| `mime.c/h` | `mime` command — MIME part structure analysis |
| `listing.c/h` | `list` command implementation |
| `utils.c/h` | Shared helpers (memory, validation, buffered reads) |
| `libs.c/h` | OpenSSL library bindings |

## Examples

### List messages in INBOX
```bash
./fetchmail -u test@comp30023 -p pass list unimelb-comp30023-2024.cloud.edu.au
```

### List messages in a specific folder
```bash
./fetchmail -u test@comp30023 -p pass -f Test list unimelb-comp30023-2024.cloud.edu.au
```

### Retrieve a message
```bash
./fetchmail -u test@comp30023 -p pass -f Test -n 1 retrieve unimelb-comp30023-2024.cloud.edu.au
```

### Parse email headers
```bash
./fetchmail -u test@comp30023 -p pass -f Test -n 2 parse unimelb-comp30023-2024.cloud.edu.au
```

### Print MIME structure
```bash
./fetchmail -u test@comp30023 -p pass -f Test -n 2 mime unimelb-comp30023-2024.cloud.edu.au
```

### Use TLS (port 993)
```bash
./fetchmail -u test@comp30023 -p pass -f Test -n 1 -t retrieve unimelb-comp30023-2024.cloud.edu.au
```

## Test Cases

```bash
# retrieve
./fetchmail -f Test -p pass -u test@comp30023 -n 1 retrieve unimelb-comp30023-2024.cloud.edu.au | diff - out/ret-ed512.out
./fetchmail -f Test -p pass -u test@comp30023 -n 2 retrieve unimelb-comp30023-2024.cloud.edu.au | diff - out/ret-mst.out
./fetchmail -f Test -u test@comp30023 -p pass1 -n 1 retrieve unimelb-comp30023-2024.cloud.edu.au | diff - out/ret-loginfail.out
./fetchmail -u test@comp30023 -p pass -n 1 -f Test1 retrieve unimelb-comp30023-2024.cloud.edu.au | diff - out/ret-nofolder.out
./fetchmail -n 42 -u test@comp30023 -p pass -f Test retrieve unimelb-comp30023-2024.cloud.edu.au | diff - out/ret-nomessage.out
./fetchmail -u test.test@comp30023 -p -p -f Test -n 1 retrieve unimelb-comp30023-2024.cloud.edu.au | diff - out/ret-mst.out
./fetchmail -f 'With Space' -n 1 -u test@comp30023 -p pass retrieve unimelb-comp30023-2024.cloud.edu.au | diff - out/ret-mst.out
./fetchmail -f more -p pass -u test@comp30023 -n 2 retrieve unimelb-comp30023-2024.cloud.edu.au | diff - out/ret-nul.out
./fetchmail -f Test -p pass -u test@comp30023 -n 1 -t retrieve unimelb-comp30023-2024.cloud.edu.au | diff - out/ret-ed512.out

# parse
./fetchmail -f Test -p pass -n 2 -u test@comp30023 parse unimelb-comp30023-2024.cloud.edu.au | diff - out/parse-mst.out
./fetchmail -f Test -n 3 -p pass -u test@comp30023 parse unimelb-comp30023-2024.cloud.edu.au | diff - out/parse-minimal.out
./fetchmail -p pass -f headers -u test@comp30023 -n 2 parse unimelb-comp30023-2024.cloud.edu.au | diff - out/parse-caps.out
./fetchmail -f headers -u test@comp30023 -p pass -n 3 parse unimelb-comp30023-2024.cloud.edu.au | diff - out/parse-nosubj.out
./fetchmail -u test@comp30023 -n 4 -p pass -f headers parse unimelb-comp30023-2024.cloud.edu.au | diff - out/parse-nested.out
./fetchmail -f headers -u test@comp30023 -n 5 -p pass parse unimelb-comp30023-2024.cloud.edu.au | diff - out/parse-ws.out
./fetchmail -f more -p pass -u test@comp30023 -n 1 parse unimelb-comp30023-2024.cloud.edu.au | diff - out/parse-mst-tab.out

# mime
./fetchmail -n 1 -p pass -u test@comp30023 mime unimelb-comp30023-2024.cloud.edu.au | diff - out/mime-ed512.out
./fetchmail -f Test -n 2 -p pass -u test@comp30023 mime unimelb-comp30023-2024.cloud.edu.au | diff - out/mime-mst.out

# list
./fetchmail -p pass -u test@comp30023 -f Test list unimelb-comp30023-2024.cloud.edu.au | diff - out/list-Test.out
./fetchmail -p pass -u test@comp30023 list unimelb-comp30023-2024.cloud.edu.au | diff - out/list-INBOX.out
```

### Invalid / edge-case inputs (should exit with failure)
```bash
./fetchmail
./fetchmail -u
./fetchmail retrieve
./fetchmail unimelb-comp30023-2024.cloud.edu.au
./fetchmail retrieve unimelb-comp30023-2024.cloud.edu.au
./fetchmail -p -u retrieve unimelb-comp30023-2024.cloud.edu.au
./fetchmail -p a -u f -f retrieve unimelb-comp30023-2024.cloud.edu.au
./fetchmail -n 0 -u test@comp30023 -p pass -f Test retrieve unimelb-comp30023-2024.cloud.edu.au
./fetchmail -n -1 -u test@comp30023 -p pass -f Test retrieve unimelb-comp30023-2024.cloud.edu.au
./fetchmail -n 10000000000000000000000000000000000 -u test@comp30023 -p pass -f Test retrieve unimelb-comp30023-2024.cloud.edu.au
./fetchmail -n comp30023 -u test@comp30023 -p pass -f Test retrieve unimelb-comp30023-2024.cloud.edu.au
./fetchmail -n 1 -u test@comp30023 -p pass -f '' retrieve unimelb-comp30023-2024.cloud.edu.au
./fetchmail -n 1 -u test@comp30023 -p pass -f ' ' retrieve unimelb-comp30023-2024.cloud.edu.au
./fetchmail -n 1 -u test@comp30023 -p pass -f '🫤' retrieve unimelb-comp30023-2024.cloud.edu.au
./fetchmail -u f -p a retrieve ''
./fetchmail -u f -p a retrieve ' '
./fetchmail -u f -p a retrieve aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
./fetchmail -f Test -p $'pass\r\nCOMP30023 LOGIN inject@comp30023 pass' -u inject@comp30023 -n 1 retrieve unimelb-comp30023-2024.cloud.edu.au
./fetchmail -f Test -p pass -u test@comp30023 -n 1 retrieve <a-server-which-disconnects>
./fetchmail -u test@comp30023 -p pass -f Test -n $'1\x2c2' retrieve unimelb-comp30023-2024.cloud.edu.au
```