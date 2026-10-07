# Project 2

- Name: Perry Aryee
- Email: perryaryee@u.boisestate.edu
- Class: CS525-002

## Known Bugs or Issues

No known issues

## Experience

The hardest part of this project was keeping the protocol logic separate from network and file I/O. 
The sender and receiver are built as pure state machines, which made them much easier to test. 
The catch was that everything the real world would normally do to them, like an ACK arriving, a timeout firing, a packet being delivered, or a retransmission, had to be modeled as function inputs and outputs.

The checksum and packet-validation code needed careful attention too. 
I had to get network byte order right, handle odd-length payloads, and never trust the length field in an incoming packet. Go-Back-N clicked once I focused on two variables, base and next. An ACK moves base, sending a packet moves next, and a timeout moves neither one. It just retransmits everything still outstanding in the window.

## Results

These measurements used a 1 MiB file, the default 250 ms retransmission
timeout, and the course relay running with `--delay 50 --seed 7`.
Each result is the mean of three successful transfers, and every output was
verified against the input with `cmp`.

| Window | Loss | Corrupt | Dup | Mean time (s) | Throughput (KiB/s) |
|---:|---:|---:|---:|---:|---:|
| 1 | 0 | 0 | 0 | 103.191 | 9.923 |
| 16 | 0 | 0 | 0 | 6.611 | 154.901 |
| 1 | 0.05 | 0 | 0 | 128.964 | 7.940 |
| 16 | 0.05 | 0 | 0 | 23.582 | 43.423 |

## Analysis

The clean window-1 mean was 103.191 seconds. The transmition of 1,024
DATA packets and one FIN, waiting one round trip for each, so its observed
round-trip time was `(103.191 / 1025) × 1000 = 100.674 ms`. The relay accounts
for 100 ms. The remaining 0.674 ms per round trip comes from process
scheduling, relay and protocol processing, socket operations, packet
serialization, and timer or measurement granularity in the Codespaces virtual
machine.

Increasing the window from 1 to 16 reduced the clean mean from 103.191 seconds
to 6.611 seconds, a 15.610× speedup. This is close to the ideal factor of 16
because the overlap is almost sixteen round trips. It is not exactly
16× because the 1,024 DATA packets require roughly 64 windowfuls, FIN requires
another round trip, and scheduling and per-packet processing cannot be
completely overlapped.

Five percent loss made the window-1 transfer 1.250× slower, but made the
window-16 transfer 3.567× slower. With window 1, a timeout resends only one
packet. With window 16, Go-Back-N resends every packet from `base` through
`next - 1`, so one loss can cause many correctly transmitted packets to be
sent again. ACK loss creates additional timeout opportunities. The larger
window remained faster under loss, but its advantage fell from 15.610× to
5.469×.