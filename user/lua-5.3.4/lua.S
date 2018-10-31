
lua.o:     file format elf32-tradlittlemips

Disassembly of section .text:

00000000 <lstop>:
       0:	3c1c0000 	lui	gp,0x0
       4:	279c0000 	addiu	gp,gp,0
       8:	0399e021 	addu	gp,gp,t9
       c:	27bdffe0 	addiu	sp,sp,-32
      10:	afbf001c 	sw	ra,28(sp)
      14:	afb00018 	sw	s0,24(sp)
      18:	afbc0010 	sw	gp,16(sp)
      1c:	8f990000 	lw	t9,0(gp)
      20:	00002821 	move	a1,zero
      24:	00003021 	move	a2,zero
      28:	00003821 	move	a3,zero
      2c:	0320f809 	jalr	t9
      30:	00808021 	move	s0,a0
      34:	8fbc0010 	lw	gp,16(sp)
      38:	02002021 	move	a0,s0
      3c:	8f850000 	lw	a1,0(gp)
      40:	8f990000 	lw	t9,0(gp)
      44:	8fbf001c 	lw	ra,28(sp)
      48:	8fb00018 	lw	s0,24(sp)
      4c:	24a50000 	addiu	a1,a1,0
      50:	03200008 	jr	t9
      54:	27bd0020 	addiu	sp,sp,32

00000058 <laction>:
      58:	3c1c0000 	lui	gp,0x0
      5c:	279c0000 	addiu	gp,gp,0
      60:	0399e021 	addu	gp,gp,t9
      64:	27bdffe0 	addiu	sp,sp,-32
      68:	afbf0018 	sw	ra,24(sp)
      6c:	afbc0010 	sw	gp,16(sp)
      70:	8f990000 	lw	t9,0(gp)
      74:	00000000 	nop
      78:	0320f809 	jalr	t9
      7c:	00002821 	move	a1,zero
      80:	8fbc0010 	lw	gp,16(sp)
      84:	8fbf0018 	lw	ra,24(sp)
      88:	8f830000 	lw	v1,0(gp)
      8c:	8f850000 	lw	a1,0(gp)
      90:	8c640000 	lw	a0,0(v1)
      94:	8f990000 	lw	t9,0(gp)
      98:	24a50000 	addiu	a1,a1,0
      9c:	2406000b 	li	a2,11
      a0:	24070001 	li	a3,1
      a4:	03200008 	jr	t9
      a8:	27bd0020 	addiu	sp,sp,32

000000ac <l_message>:
      ac:	3c1c0000 	lui	gp,0x0
      b0:	279c0000 	addiu	gp,gp,0
      b4:	0399e021 	addu	gp,gp,t9
      b8:	27bdffd8 	addiu	sp,sp,-40
      bc:	afbf0020 	sw	ra,32(sp)
      c0:	afb1001c 	sw	s1,28(sp)
      c4:	afb00018 	sw	s0,24(sp)
      c8:	afbc0010 	sw	gp,16(sp)
      cc:	00a08821 	move	s1,a1
      d0:	8f850000 	lw	a1,0(gp)
      d4:	8f990000 	lw	t9,0(gp)
      d8:	00803021 	move	a2,a0
      dc:	8f900000 	lw	s0,0(gp)
      e0:	1080000c 	beqz	a0,114 <l_message+0x68>
      e4:	24a50000 	addiu	a1,a1,0
      e8:	8e040000 	lw	a0,0(s0)
      ec:	0320f809 	jalr	t9
      f0:	00000000 	nop
      f4:	8fbc0010 	lw	gp,16(sp)
      f8:	8e040000 	lw	a0,0(s0)
      fc:	8f990000 	lw	t9,0(gp)
     100:	00000000 	nop
     104:	0320f809 	jalr	t9
     108:	00000000 	nop
     10c:	8fbc0010 	lw	gp,16(sp)
     110:	00000000 	nop
     114:	8f850000 	lw	a1,0(gp)
     118:	8f990000 	lw	t9,0(gp)
     11c:	8e040000 	lw	a0,0(s0)
     120:	02203021 	move	a2,s1
     124:	0320f809 	jalr	t9
     128:	24a50000 	addiu	a1,a1,0
     12c:	8fbc0010 	lw	gp,16(sp)
     130:	8e040000 	lw	a0,0(s0)
     134:	8f990000 	lw	t9,0(gp)
     138:	8fbf0020 	lw	ra,32(sp)
     13c:	8fb1001c 	lw	s1,28(sp)
     140:	8fb00018 	lw	s0,24(sp)
     144:	03200008 	jr	t9
     148:	27bd0028 	addiu	sp,sp,40

0000014c <report>:
     14c:	3c1c0000 	lui	gp,0x0
     150:	279c0000 	addiu	gp,gp,0
     154:	0399e021 	addu	gp,gp,t9
     158:	27bdffd8 	addiu	sp,sp,-40
     15c:	afbf0020 	sw	ra,32(sp)
     160:	afb1001c 	sw	s1,28(sp)
     164:	afb00018 	sw	s0,24(sp)
     168:	afbc0010 	sw	gp,16(sp)
     16c:	00a08021 	move	s0,a1
     170:	8f990000 	lw	t9,0(gp)
     174:	00003021 	move	a2,zero
     178:	2405ffff 	li	a1,-1
     17c:	16000007 	bnez	s0,19c <report+0x50>
     180:	00808821 	move	s1,a0
     184:	02001021 	move	v0,s0
     188:	8fbf0020 	lw	ra,32(sp)
     18c:	8fb1001c 	lw	s1,28(sp)
     190:	8fb00018 	lw	s0,24(sp)
     194:	03e00008 	jr	ra
     198:	27bd0028 	addiu	sp,sp,40
     19c:	0320f809 	jalr	t9
     1a0:	00000000 	nop
     1a4:	8fbc0010 	lw	gp,16(sp)
     1a8:	00000000 	nop
     1ac:	8f830000 	lw	v1,0(gp)
     1b0:	8f990000 	lw	t9,0(gp)
     1b4:	8c640000 	lw	a0,0(v1)
     1b8:	273900ac 	addiu	t9,t9,172
     1bc:	0320f809 	jalr	t9
     1c0:	00402821 	move	a1,v0
     1c4:	8fbc0010 	lw	gp,16(sp)
     1c8:	02202021 	move	a0,s1
     1cc:	8f990000 	lw	t9,0(gp)
     1d0:	00000000 	nop
     1d4:	0320f809 	jalr	t9
     1d8:	2405fffe 	li	a1,-2
     1dc:	02001021 	move	v0,s0
     1e0:	8fbc0010 	lw	gp,16(sp)
     1e4:	8fbf0020 	lw	ra,32(sp)
     1e8:	8fb1001c 	lw	s1,28(sp)
     1ec:	8fb00018 	lw	s0,24(sp)
     1f0:	03e00008 	jr	ra
     1f4:	27bd0028 	addiu	sp,sp,40

000001f8 <msghandler>:
     1f8:	3c1c0000 	lui	gp,0x0
     1fc:	279c0000 	addiu	gp,gp,0
     200:	0399e021 	addu	gp,gp,t9
     204:	27bdffe0 	addiu	sp,sp,-32
     208:	afbf001c 	sw	ra,28(sp)
     20c:	afb00018 	sw	s0,24(sp)
     210:	afbc0010 	sw	gp,16(sp)
     214:	8f990000 	lw	t9,0(gp)
     218:	24050001 	li	a1,1
     21c:	00003021 	move	a2,zero
     220:	0320f809 	jalr	t9
     224:	00808021 	move	s0,a0
     228:	8fbc0010 	lw	gp,16(sp)
     22c:	02002021 	move	a0,s0
     230:	8f860000 	lw	a2,0(gp)
     234:	8f990000 	lw	t9,0(gp)
     238:	24c60000 	addiu	a2,a2,0
     23c:	1040000d 	beqz	v0,274 <msghandler+0x7c>
     240:	24050001 	li	a1,1
     244:	8f990000 	lw	t9,0(gp)
     248:	02002021 	move	a0,s0
     24c:	00403021 	move	a2,v0
     250:	02002821 	move	a1,s0
     254:	0320f809 	jalr	t9
     258:	24070001 	li	a3,1
     25c:	8fbc0010 	lw	gp,16(sp)
     260:	8fbf001c 	lw	ra,28(sp)
     264:	8fb00018 	lw	s0,24(sp)
     268:	24020001 	li	v0,1
     26c:	03e00008 	jr	ra
     270:	27bd0020 	addiu	sp,sp,32
     274:	0320f809 	jalr	t9
     278:	00000000 	nop
     27c:	8fbc0010 	lw	gp,16(sp)
     280:	02002021 	move	a0,s0
     284:	8f990000 	lw	t9,0(gp)
     288:	10400009 	beqz	v0,2b0 <msghandler+0xb8>
     28c:	2405ffff 	li	a1,-1
     290:	0320f809 	jalr	t9
     294:	00000000 	nop
     298:	24030004 	li	v1,4
     29c:	8fbc0010 	lw	gp,16(sp)
     2a0:	1043ffef 	beq	v0,v1,260 <msghandler+0x68>
     2a4:	02002021 	move	a0,s0
     2a8:	8f990000 	lw	t9,0(gp)
     2ac:	00000000 	nop
     2b0:	0320f809 	jalr	t9
     2b4:	24050001 	li	a1,1
     2b8:	8fbc0010 	lw	gp,16(sp)
     2bc:	02002021 	move	a0,s0
     2c0:	8f990000 	lw	t9,0(gp)
     2c4:	00000000 	nop
     2c8:	0320f809 	jalr	t9
     2cc:	00402821 	move	a1,v0
     2d0:	8fbc0010 	lw	gp,16(sp)
     2d4:	02002021 	move	a0,s0
     2d8:	8f850000 	lw	a1,0(gp)
     2dc:	8f990000 	lw	t9,0(gp)
     2e0:	24a50000 	addiu	a1,a1,0
     2e4:	0320f809 	jalr	t9
     2e8:	00403021 	move	a2,v0
     2ec:	8fbc0010 	lw	gp,16(sp)
     2f0:	1000ffd4 	b	244 <msghandler+0x4c>
     2f4:	00000000 	nop

000002f8 <docall>:
     2f8:	3c1c0000 	lui	gp,0x0
     2fc:	279c0000 	addiu	gp,gp,0
     300:	0399e021 	addu	gp,gp,t9
     304:	27bdffc8 	addiu	sp,sp,-56
     308:	afbf0030 	sw	ra,48(sp)
     30c:	afb3002c 	sw	s3,44(sp)
     310:	afb20028 	sw	s2,40(sp)
     314:	afb10024 	sw	s1,36(sp)
     318:	afb00020 	sw	s0,32(sp)
     31c:	afbc0018 	sw	gp,24(sp)
     320:	8f990000 	lw	t9,0(gp)
     324:	00808021 	move	s0,a0
     328:	00a09021 	move	s2,a1
     32c:	0320f809 	jalr	t9
     330:	00c09821 	move	s3,a2
     334:	8fbc0018 	lw	gp,24(sp)
     338:	02002021 	move	a0,s0
     33c:	8f850000 	lw	a1,0(gp)
     340:	8f990000 	lw	t9,0(gp)
     344:	24a501f8 	addiu	a1,a1,504
     348:	00003021 	move	a2,zero
     34c:	0320f809 	jalr	t9
     350:	00528823 	subu	s1,v0,s2
     354:	8fbc0018 	lw	gp,24(sp)
     358:	02002021 	move	a0,s0
     35c:	8f990000 	lw	t9,0(gp)
     360:	02202821 	move	a1,s1
     364:	0320f809 	jalr	t9
     368:	24060001 	li	a2,1
     36c:	8fbc0018 	lw	gp,24(sp)
     370:	24040002 	li	a0,2
     374:	8f820000 	lw	v0,0(gp)
     378:	8f850000 	lw	a1,0(gp)
     37c:	8f990000 	lw	t9,0(gp)
     380:	ac500000 	sw	s0,0(v0)
     384:	0320f809 	jalr	t9
     388:	24a50058 	addiu	a1,a1,88
     38c:	8fbc0018 	lw	gp,24(sp)
     390:	02203821 	move	a3,s1
     394:	8f990000 	lw	t9,0(gp)
     398:	02603021 	move	a2,s3
     39c:	02402821 	move	a1,s2
     3a0:	02002021 	move	a0,s0
     3a4:	afa00010 	sw	zero,16(sp)
     3a8:	0320f809 	jalr	t9
     3ac:	afa00014 	sw	zero,20(sp)
     3b0:	8fbc0018 	lw	gp,24(sp)
     3b4:	24040002 	li	a0,2
     3b8:	8f990000 	lw	t9,0(gp)
     3bc:	00002821 	move	a1,zero
     3c0:	0320f809 	jalr	t9
     3c4:	00409021 	move	s2,v0
     3c8:	8fbc0018 	lw	gp,24(sp)
     3cc:	02002021 	move	a0,s0
     3d0:	8f990000 	lw	t9,0(gp)
     3d4:	02202821 	move	a1,s1
     3d8:	0320f809 	jalr	t9
     3dc:	2406ffff 	li	a2,-1
     3e0:	8fbc0018 	lw	gp,24(sp)
     3e4:	02002021 	move	a0,s0
     3e8:	8f990000 	lw	t9,0(gp)
     3ec:	00000000 	nop
     3f0:	0320f809 	jalr	t9
     3f4:	2405fffe 	li	a1,-2
     3f8:	02401021 	move	v0,s2
     3fc:	8fbc0018 	lw	gp,24(sp)
     400:	8fbf0030 	lw	ra,48(sp)
     404:	8fb3002c 	lw	s3,44(sp)
     408:	8fb20028 	lw	s2,40(sp)
     40c:	8fb10024 	lw	s1,36(sp)
     410:	8fb00020 	lw	s0,32(sp)
     414:	03e00008 	jr	ra
     418:	27bd0038 	addiu	sp,sp,56

0000041c <print_version>:
     41c:	3c1c0000 	lui	gp,0x0
     420:	279c0000 	addiu	gp,gp,0
     424:	0399e021 	addu	gp,gp,t9
     428:	27bdffe0 	addiu	sp,sp,-32
     42c:	afbf001c 	sw	ra,28(sp)
     430:	afb00018 	sw	s0,24(sp)
     434:	afbc0010 	sw	gp,16(sp)
     438:	8f900000 	lw	s0,0(gp)
     43c:	8f840000 	lw	a0,0(gp)
     440:	8f990000 	lw	t9,0(gp)
     444:	8e070000 	lw	a3,0(s0)
     448:	24840000 	addiu	a0,a0,0
     44c:	24050001 	li	a1,1
     450:	0320f809 	jalr	t9
     454:	24060033 	li	a2,51
     458:	8fbc0010 	lw	gp,16(sp)
     45c:	8e070000 	lw	a3,0(s0)
     460:	8f840000 	lw	a0,0(gp)
     464:	8f990000 	lw	t9,0(gp)
     468:	24840000 	addiu	a0,a0,0
     46c:	24050001 	li	a1,1
     470:	0320f809 	jalr	t9
     474:	24060001 	li	a2,1
     478:	8fbc0010 	lw	gp,16(sp)
     47c:	8e040000 	lw	a0,0(s0)
     480:	8f990000 	lw	t9,0(gp)
     484:	8fbf001c 	lw	ra,28(sp)
     488:	8fb00018 	lw	s0,24(sp)
     48c:	03200008 	jr	t9
     490:	27bd0020 	addiu	sp,sp,32

00000494 <dochunk>:
     494:	3c1c0000 	lui	gp,0x0
     498:	279c0000 	addiu	gp,gp,0
     49c:	0399e021 	addu	gp,gp,t9
     4a0:	27bdffe0 	addiu	sp,sp,-32
     4a4:	afbf001c 	sw	ra,28(sp)
     4a8:	afb00018 	sw	s0,24(sp)
     4ac:	afbc0010 	sw	gp,16(sp)
     4b0:	00a01021 	move	v0,a1
     4b4:	00003021 	move	a2,zero
     4b8:	10a00009 	beqz	a1,4e0 <dochunk+0x4c>
     4bc:	00808021 	move	s0,a0
     4c0:	8f990000 	lw	t9,0(gp)
     4c4:	02002021 	move	a0,s0
     4c8:	8fbf001c 	lw	ra,28(sp)
     4cc:	8fb00018 	lw	s0,24(sp)
     4d0:	00402821 	move	a1,v0
     4d4:	2739014c 	addiu	t9,t9,332
     4d8:	03200008 	jr	t9
     4dc:	27bd0020 	addiu	sp,sp,32
     4e0:	8f990000 	lw	t9,0(gp)
     4e4:	00000000 	nop
     4e8:	273902f8 	addiu	t9,t9,760
     4ec:	0320f809 	jalr	t9
     4f0:	00000000 	nop
     4f4:	8fbc0010 	lw	gp,16(sp)
     4f8:	02002021 	move	a0,s0
     4fc:	8f990000 	lw	t9,0(gp)
     500:	8fbf001c 	lw	ra,28(sp)
     504:	8fb00018 	lw	s0,24(sp)
     508:	00402821 	move	a1,v0
     50c:	2739014c 	addiu	t9,t9,332
     510:	03200008 	jr	t9
     514:	27bd0020 	addiu	sp,sp,32

00000518 <dofile>:
     518:	3c1c0000 	lui	gp,0x0
     51c:	279c0000 	addiu	gp,gp,0
     520:	0399e021 	addu	gp,gp,t9
     524:	27bdffe0 	addiu	sp,sp,-32
     528:	afbf001c 	sw	ra,28(sp)
     52c:	afb00018 	sw	s0,24(sp)
     530:	afbc0010 	sw	gp,16(sp)
     534:	8f990000 	lw	t9,0(gp)
     538:	00003021 	move	a2,zero
     53c:	0320f809 	jalr	t9
     540:	00808021 	move	s0,a0
     544:	8fbc0010 	lw	gp,16(sp)
     548:	02002021 	move	a0,s0
     54c:	8f990000 	lw	t9,0(gp)
     550:	8fbf001c 	lw	ra,28(sp)
     554:	8fb00018 	lw	s0,24(sp)
     558:	00402821 	move	a1,v0
     55c:	27390494 	addiu	t9,t9,1172
     560:	03200008 	jr	t9
     564:	27bd0020 	addiu	sp,sp,32

00000568 <dostring>:
     568:	3c1c0000 	lui	gp,0x0
     56c:	279c0000 	addiu	gp,gp,0
     570:	0399e021 	addu	gp,gp,t9
     574:	27bdffd0 	addiu	sp,sp,-48
     578:	afbf002c 	sw	ra,44(sp)
     57c:	afb20028 	sw	s2,40(sp)
     580:	afb10024 	sw	s1,36(sp)
     584:	afb00020 	sw	s0,32(sp)
     588:	afbc0018 	sw	gp,24(sp)
     58c:	8f990000 	lw	t9,0(gp)
     590:	00809021 	move	s2,a0
     594:	00a02021 	move	a0,a1
     598:	00c08821 	move	s1,a2
     59c:	0320f809 	jalr	t9
     5a0:	00a08021 	move	s0,a1
     5a4:	8fbc0018 	lw	gp,24(sp)
     5a8:	02002821 	move	a1,s0
     5ac:	8f990000 	lw	t9,0(gp)
     5b0:	02402021 	move	a0,s2
     5b4:	afa00010 	sw	zero,16(sp)
     5b8:	02203821 	move	a3,s1
     5bc:	0320f809 	jalr	t9
     5c0:	00403021 	move	a2,v0
     5c4:	8fbc0018 	lw	gp,24(sp)
     5c8:	02402021 	move	a0,s2
     5cc:	8f990000 	lw	t9,0(gp)
     5d0:	8fbf002c 	lw	ra,44(sp)
     5d4:	8fb20028 	lw	s2,40(sp)
     5d8:	8fb10024 	lw	s1,36(sp)
     5dc:	8fb00020 	lw	s0,32(sp)
     5e0:	00402821 	move	a1,v0
     5e4:	27390494 	addiu	t9,t9,1172
     5e8:	03200008 	jr	t9
     5ec:	27bd0030 	addiu	sp,sp,48

000005f0 <pushline>:
     5f0:	3c1c0000 	lui	gp,0x0
     5f4:	279c0000 	addiu	gp,gp,0
     5f8:	0399e021 	addu	gp,gp,t9
     5fc:	27bdfdd0 	addiu	sp,sp,-560
     600:	afbf0228 	sw	ra,552(sp)
     604:	afb30224 	sw	s3,548(sp)
     608:	afb20220 	sw	s2,544(sp)
     60c:	afb1021c 	sw	s1,540(sp)
     610:	afb00218 	sw	s0,536(sp)
     614:	afbc0010 	sw	gp,16(sp)
     618:	00a09821 	move	s3,a1
     61c:	00809021 	move	s2,a0
     620:	10a0002c 	beqz	a1,6d4 <pushline+0xe4>
     624:	27b10018 	addiu	s1,sp,24
     628:	8f820000 	lw	v0,0(gp)
     62c:	8f990000 	lw	t9,0(gp)
     630:	24450000 	addiu	a1,v0,0
     634:	0320f809 	jalr	t9
     638:	02402021 	move	a0,s2
     63c:	8fbc0010 	lw	gp,16(sp)
     640:	02402021 	move	a0,s2
     644:	8f990000 	lw	t9,0(gp)
     648:	2405ffff 	li	a1,-1
     64c:	0320f809 	jalr	t9
     650:	00003021 	move	a2,zero
     654:	8fbc0010 	lw	gp,16(sp)
     658:	1040002c 	beqz	v0,70c <pushline+0x11c>
     65c:	00000000 	nop
     660:	8f900000 	lw	s0,0(gp)
     664:	8f990000 	lw	t9,0(gp)
     668:	8e050000 	lw	a1,0(s0)
     66c:	0320f809 	jalr	t9
     670:	00402021 	move	a0,v0
     674:	8fbc0010 	lw	gp,16(sp)
     678:	8e040000 	lw	a0,0(s0)
     67c:	8f990000 	lw	t9,0(gp)
     680:	00000000 	nop
     684:	0320f809 	jalr	t9
     688:	00000000 	nop
     68c:	8fbc0010 	lw	gp,16(sp)
     690:	02202021 	move	a0,s1
     694:	8f820000 	lw	v0,0(gp)
     698:	8f990000 	lw	t9,0(gp)
     69c:	8c460000 	lw	a2,0(v0)
     6a0:	0320f809 	jalr	t9
     6a4:	24050200 	li	a1,512
     6a8:	8fbc0010 	lw	gp,16(sp)
     6ac:	1440001c 	bnez	v0,720 <pushline+0x130>
     6b0:	00001821 	move	v1,zero
     6b4:	8fbf0228 	lw	ra,552(sp)
     6b8:	8fb30224 	lw	s3,548(sp)
     6bc:	8fb20220 	lw	s2,544(sp)
     6c0:	8fb1021c 	lw	s1,540(sp)
     6c4:	8fb00218 	lw	s0,536(sp)
     6c8:	00601021 	move	v0,v1
     6cc:	03e00008 	jr	ra
     6d0:	27bd0230 	addiu	sp,sp,560
     6d4:	8f820000 	lw	v0,0(gp)
     6d8:	8f990000 	lw	t9,0(gp)
     6dc:	24450000 	addiu	a1,v0,0
     6e0:	0320f809 	jalr	t9
     6e4:	02402021 	move	a0,s2
     6e8:	8fbc0010 	lw	gp,16(sp)
     6ec:	02402021 	move	a0,s2
     6f0:	8f990000 	lw	t9,0(gp)
     6f4:	2405ffff 	li	a1,-1
     6f8:	0320f809 	jalr	t9
     6fc:	00003021 	move	a2,zero
     700:	8fbc0010 	lw	gp,16(sp)
     704:	1440ffd6 	bnez	v0,660 <pushline+0x70>
     708:	00000000 	nop
     70c:	1260002e 	beqz	s3,7c8 <pushline+0x1d8>
     710:	00000000 	nop
     714:	8f820000 	lw	v0,0(gp)
     718:	1000ffd1 	b	660 <pushline+0x70>
     71c:	24420000 	addiu	v0,v0,0
     720:	8f990000 	lw	t9,0(gp)
     724:	02402021 	move	a0,s2
     728:	0320f809 	jalr	t9
     72c:	2405fffe 	li	a1,-2
     730:	8fbc0010 	lw	gp,16(sp)
     734:	00000000 	nop
     738:	8f990000 	lw	t9,0(gp)
     73c:	00000000 	nop
     740:	0320f809 	jalr	t9
     744:	02202021 	move	a0,s1
     748:	8fbc0010 	lw	gp,16(sp)
     74c:	14400015 	bnez	v0,7a4 <pushline+0x1b4>
     750:	00403021 	move	a2,v0
     754:	12600005 	beqz	s3,76c <pushline+0x17c>
     758:	2402003d 	li	v0,61
     75c:	82230000 	lb	v1,0(s1)
     760:	00000000 	nop
     764:	1062001b 	beq	v1,v0,7d4 <pushline+0x1e4>
     768:	02402021 	move	a0,s2
     76c:	8f990000 	lw	t9,0(gp)
     770:	02402021 	move	a0,s2
     774:	0320f809 	jalr	t9
     778:	02202821 	move	a1,s1
     77c:	8fbc0010 	lw	gp,16(sp)
     780:	24030001 	li	v1,1
     784:	8fbf0228 	lw	ra,552(sp)
     788:	8fb30224 	lw	s3,548(sp)
     78c:	8fb20220 	lw	s2,544(sp)
     790:	8fb1021c 	lw	s1,540(sp)
     794:	8fb00218 	lw	s0,536(sp)
     798:	00601021 	move	v0,v1
     79c:	03e00008 	jr	ra
     7a0:	27bd0230 	addiu	sp,sp,560
     7a4:	02221021 	addu	v0,s1,v0
     7a8:	8044ffff 	lb	a0,-1(v0)
     7ac:	2403000a 	li	v1,10
     7b0:	1483ffe8 	bne	a0,v1,754 <pushline+0x164>
     7b4:	00000000 	nop
     7b8:	24c6ffff 	addiu	a2,a2,-1
     7bc:	00d11021 	addu	v0,a2,s1
     7c0:	1000ffe4 	b	754 <pushline+0x164>
     7c4:	a0400000 	sb	zero,0(v0)
     7c8:	8f820000 	lw	v0,0(gp)
     7cc:	1000ffa4 	b	660 <pushline+0x70>
     7d0:	24420000 	addiu	v0,v0,0
     7d4:	8f850000 	lw	a1,0(gp)
     7d8:	8f990000 	lw	t9,0(gp)
     7dc:	24a50000 	addiu	a1,a1,0
     7e0:	0320f809 	jalr	t9
     7e4:	26260001 	addiu	a2,s1,1
     7e8:	8fbc0010 	lw	gp,16(sp)
     7ec:	1000ffe5 	b	784 <pushline+0x194>
     7f0:	24030001 	li	v1,1

000007f4 <doREPL>:
     7f4:	3c1c0000 	lui	gp,0x0
     7f8:	279c0000 	addiu	gp,gp,0
     7fc:	0399e021 	addu	gp,gp,t9
     800:	27bdffa8 	addiu	sp,sp,-88
     804:	afbf0054 	sw	ra,84(sp)
     808:	afbe0050 	sw	s8,80(sp)
     80c:	afb7004c 	sw	s7,76(sp)
     810:	afb60048 	sw	s6,72(sp)
     814:	afb50044 	sw	s5,68(sp)
     818:	afb40040 	sw	s4,64(sp)
     81c:	afb3003c 	sw	s3,60(sp)
     820:	afb20038 	sw	s2,56(sp)
     824:	afb10034 	sw	s1,52(sp)
     828:	afb00030 	sw	s0,48(sp)
     82c:	afbc0018 	sw	gp,24(sp)
     830:	8f9e0000 	lw	s8,0(gp)
     834:	8f820000 	lw	v0,0(gp)
     838:	8f830000 	lw	v1,0(gp)
     83c:	245505f0 	addiu	s5,v0,1520
     840:	8fc20000 	lw	v0,0(s8)
     844:	8f970000 	lw	s7,0(gp)
     848:	00808821 	move	s1,a0
     84c:	2476014c 	addiu	s6,v1,332
     850:	afa20028 	sw	v0,40(sp)
     854:	afc00000 	sw	zero,0(s8)
     858:	8f990000 	lw	t9,0(gp)
     85c:	02202021 	move	a0,s1
     860:	0320f809 	jalr	t9
     864:	00002821 	move	a1,zero
     868:	24050001 	li	a1,1
     86c:	02a0c821 	move	t9,s5
     870:	0320f809 	jalr	t9
     874:	02202021 	move	a0,s1
     878:	8fbc0018 	lw	gp,24(sp)
     87c:	1440001e 	bnez	v0,8f8 <doREPL+0x104>
     880:	2405ffff 	li	a1,-1
     884:	2402ffff 	li	v0,-1
     888:	10a20054 	beq	a1,v0,9dc <doREPL+0x1e8>
     88c:	00000000 	nop
     890:	14a0000a 	bnez	a1,8bc <doREPL+0xc8>
     894:	02c0c821 	move	t9,s6
     898:	8f990000 	lw	t9,0(gp)
     89c:	02202021 	move	a0,s1
     8a0:	273902f8 	addiu	t9,t9,760
     8a4:	0320f809 	jalr	t9
     8a8:	2406ffff 	li	a2,-1
     8ac:	8fbc0018 	lw	gp,24(sp)
     8b0:	1040006b 	beqz	v0,a60 <doREPL+0x26c>
     8b4:	00402821 	move	a1,v0
     8b8:	02c0c821 	move	t9,s6
     8bc:	0320f809 	jalr	t9
     8c0:	02202021 	move	a0,s1
     8c4:	8fbc0018 	lw	gp,24(sp)
     8c8:	00000000 	nop
     8cc:	8f990000 	lw	t9,0(gp)
     8d0:	02202021 	move	a0,s1
     8d4:	0320f809 	jalr	t9
     8d8:	00002821 	move	a1,zero
     8dc:	24050001 	li	a1,1
     8e0:	02a0c821 	move	t9,s5
     8e4:	0320f809 	jalr	t9
     8e8:	02202021 	move	a0,s1
     8ec:	8fbc0018 	lw	gp,24(sp)
     8f0:	1040ffe4 	beqz	v0,884 <doREPL+0x90>
     8f4:	2405ffff 	li	a1,-1
     8f8:	8f990000 	lw	t9,0(gp)
     8fc:	02202021 	move	a0,s1
     900:	0320f809 	jalr	t9
     904:	00003021 	move	a2,zero
     908:	8fbc0018 	lw	gp,24(sp)
     90c:	00403021 	move	a2,v0
     910:	8f850000 	lw	a1,0(gp)
     914:	8f990000 	lw	t9,0(gp)
     918:	24a50000 	addiu	a1,a1,0
     91c:	0320f809 	jalr	t9
     920:	02202021 	move	a0,s1
     924:	8fbc0018 	lw	gp,24(sp)
     928:	00402021 	move	a0,v0
     92c:	8f990000 	lw	t9,0(gp)
     930:	00000000 	nop
     934:	0320f809 	jalr	t9
     938:	00408021 	move	s0,v0
     93c:	8fbc0018 	lw	gp,24(sp)
     940:	afa00010 	sw	zero,16(sp)
     944:	8f920000 	lw	s2,0(gp)
     948:	8f990000 	lw	t9,0(gp)
     94c:	02002821 	move	a1,s0
     950:	00403021 	move	a2,v0
     954:	02202021 	move	a0,s1
     958:	0320f809 	jalr	t9
     95c:	26470000 	addiu	a3,s2,0
     960:	8fbc0018 	lw	gp,24(sp)
     964:	1440007a 	bnez	v0,b50 <doREPL+0x35c>
     968:	02202021 	move	a0,s1
     96c:	8f990000 	lw	t9,0(gp)
     970:	2405fffe 	li	a1,-2
     974:	0320f809 	jalr	t9
     978:	2406ffff 	li	a2,-1
     97c:	8fbc0018 	lw	gp,24(sp)
     980:	02202021 	move	a0,s1
     984:	8f990000 	lw	t9,0(gp)
     988:	00000000 	nop
     98c:	0320f809 	jalr	t9
     990:	2405fffe 	li	a1,-2
     994:	8fbc0018 	lw	gp,24(sp)
     998:	00008021 	move	s0,zero
     99c:	8f990000 	lw	t9,0(gp)
     9a0:	02202021 	move	a0,s1
     9a4:	24050001 	li	a1,1
     9a8:	0320f809 	jalr	t9
     9ac:	2406ffff 	li	a2,-1
     9b0:	8fbc0018 	lw	gp,24(sp)
     9b4:	2405fffe 	li	a1,-2
     9b8:	8f990000 	lw	t9,0(gp)
     9bc:	00000000 	nop
     9c0:	0320f809 	jalr	t9
     9c4:	02202021 	move	a0,s1
     9c8:	02002821 	move	a1,s0
     9cc:	2402ffff 	li	v0,-1
     9d0:	8fbc0018 	lw	gp,24(sp)
     9d4:	14a2ffae 	bne	a1,v0,890 <doREPL+0x9c>
     9d8:	00000000 	nop
     9dc:	8f990000 	lw	t9,0(gp)
     9e0:	02202021 	move	a0,s1
     9e4:	8f900000 	lw	s0,0(gp)
     9e8:	0320f809 	jalr	t9
     9ec:	00002821 	move	a1,zero
     9f0:	8fbc0018 	lw	gp,24(sp)
     9f4:	8e070000 	lw	a3,0(s0)
     9f8:	8f990000 	lw	t9,0(gp)
     9fc:	26e40000 	addiu	a0,s7,0
     a00:	24050001 	li	a1,1
     a04:	0320f809 	jalr	t9
     a08:	24060001 	li	a2,1
     a0c:	8fbc0018 	lw	gp,24(sp)
     a10:	8e040000 	lw	a0,0(s0)
     a14:	8f990000 	lw	t9,0(gp)
     a18:	00000000 	nop
     a1c:	0320f809 	jalr	t9
     a20:	00000000 	nop
     a24:	8fa20028 	lw	v0,40(sp)
     a28:	8fbc0018 	lw	gp,24(sp)
     a2c:	afc20000 	sw	v0,0(s8)
     a30:	8fbf0054 	lw	ra,84(sp)
     a34:	8fbe0050 	lw	s8,80(sp)
     a38:	8fb7004c 	lw	s7,76(sp)
     a3c:	8fb60048 	lw	s6,72(sp)
     a40:	8fb50044 	lw	s5,68(sp)
     a44:	8fb40040 	lw	s4,64(sp)
     a48:	8fb3003c 	lw	s3,60(sp)
     a4c:	8fb20038 	lw	s2,56(sp)
     a50:	8fb10034 	lw	s1,52(sp)
     a54:	8fb00030 	lw	s0,48(sp)
     a58:	03e00008 	jr	ra
     a5c:	27bd0058 	addiu	sp,sp,88
     a60:	8f990000 	lw	t9,0(gp)
     a64:	00000000 	nop
     a68:	0320f809 	jalr	t9
     a6c:	02202021 	move	a0,s1
     a70:	8fbc0018 	lw	gp,24(sp)
     a74:	1840ff78 	blez	v0,858 <doREPL+0x64>
     a78:	00408021 	move	s0,v0
     a7c:	8f860000 	lw	a2,0(gp)
     a80:	8f990000 	lw	t9,0(gp)
     a84:	24c60000 	addiu	a2,a2,0
     a88:	02202021 	move	a0,s1
     a8c:	0320f809 	jalr	t9
     a90:	24050014 	li	a1,20
     a94:	8fbc0018 	lw	gp,24(sp)
     a98:	02202021 	move	a0,s1
     a9c:	8f850000 	lw	a1,0(gp)
     aa0:	8f990000 	lw	t9,0(gp)
     aa4:	00000000 	nop
     aa8:	0320f809 	jalr	t9
     aac:	24a50000 	addiu	a1,a1,0
     ab0:	8fbc0018 	lw	gp,24(sp)
     ab4:	02202021 	move	a0,s1
     ab8:	8f990000 	lw	t9,0(gp)
     abc:	24050001 	li	a1,1
     ac0:	0320f809 	jalr	t9
     ac4:	24060001 	li	a2,1
     ac8:	8fbc0018 	lw	gp,24(sp)
     acc:	afa00010 	sw	zero,16(sp)
     ad0:	8f990000 	lw	t9,0(gp)
     ad4:	afa00014 	sw	zero,20(sp)
     ad8:	02002821 	move	a1,s0
     adc:	02202021 	move	a0,s1
     ae0:	00003021 	move	a2,zero
     ae4:	0320f809 	jalr	t9
     ae8:	00003821 	move	a3,zero
     aec:	8fbc0018 	lw	gp,24(sp)
     af0:	1040ff59 	beqz	v0,858 <doREPL+0x64>
     af4:	00000000 	nop
     af8:	8f990000 	lw	t9,0(gp)
     afc:	02202021 	move	a0,s1
     b00:	2405ffff 	li	a1,-1
     b04:	0320f809 	jalr	t9
     b08:	00003021 	move	a2,zero
     b0c:	8fbc0018 	lw	gp,24(sp)
     b10:	02202021 	move	a0,s1
     b14:	8f850000 	lw	a1,0(gp)
     b18:	8f990000 	lw	t9,0(gp)
     b1c:	24a50000 	addiu	a1,a1,0
     b20:	0320f809 	jalr	t9
     b24:	00403021 	move	a2,v0
     b28:	8fbc0018 	lw	gp,24(sp)
     b2c:	8fc40000 	lw	a0,0(s8)
     b30:	8f990000 	lw	t9,0(gp)
     b34:	00000000 	nop
     b38:	273900ac 	addiu	t9,t9,172
     b3c:	0320f809 	jalr	t9
     b40:	00402821 	move	a1,v0
     b44:	8fbc0018 	lw	gp,24(sp)
     b48:	1000ff60 	b	8cc <doREPL+0xd8>
     b4c:	00000000 	nop
     b50:	8f990000 	lw	t9,0(gp)
     b54:	8f940000 	lw	s4,0(gp)
     b58:	0320f809 	jalr	t9
     b5c:	2405fffd 	li	a1,-3
     b60:	8fbc0018 	lw	gp,24(sp)
     b64:	1000002d 	b	c1c <doREPL+0x428>
     b68:	24130003 	li	s3,3
     b6c:	0320f809 	jalr	t9
     b70:	00000000 	nop
     b74:	8fa30024 	lw	v1,36(sp)
     b78:	8fbc0018 	lw	gp,24(sp)
     b7c:	00431021 	addu	v0,v0,v1
     b80:	2c630005 	sltiu	v1,v1,5
     b84:	8f990000 	lw	t9,0(gp)
     b88:	26850000 	addiu	a1,s4,0
     b8c:	1460ff83 	bnez	v1,99c <doREPL+0x1a8>
     b90:	2444fffb 	addiu	a0,v0,-5
     b94:	0320f809 	jalr	t9
     b98:	00000000 	nop
     b9c:	8fbc0018 	lw	gp,24(sp)
     ba0:	02202021 	move	a0,s1
     ba4:	8f990000 	lw	t9,0(gp)
     ba8:	1440ff7c 	bnez	v0,99c <doREPL+0x1a8>
     bac:	2405fffe 	li	a1,-2
     bb0:	0320f809 	jalr	t9
     bb4:	00000000 	nop
     bb8:	02202021 	move	a0,s1
     bbc:	02a0c821 	move	t9,s5
     bc0:	0320f809 	jalr	t9
     bc4:	00002821 	move	a1,zero
     bc8:	8fbc0018 	lw	gp,24(sp)
     bcc:	02202021 	move	a0,s1
     bd0:	8f990000 	lw	t9,0(gp)
     bd4:	1040ff71 	beqz	v0,99c <doREPL+0x1a8>
     bd8:	26e50000 	addiu	a1,s7,0
     bdc:	0320f809 	jalr	t9
     be0:	00000000 	nop
     be4:	8fbc0018 	lw	gp,24(sp)
     be8:	02202021 	move	a0,s1
     bec:	8f990000 	lw	t9,0(gp)
     bf0:	2405fffe 	li	a1,-2
     bf4:	0320f809 	jalr	t9
     bf8:	24060001 	li	a2,1
     bfc:	8fbc0018 	lw	gp,24(sp)
     c00:	02202021 	move	a0,s1
     c04:	8f990000 	lw	t9,0(gp)
     c08:	00000000 	nop
     c0c:	0320f809 	jalr	t9
     c10:	24050003 	li	a1,3
     c14:	8fbc0018 	lw	gp,24(sp)
     c18:	00000000 	nop
     c1c:	8f990000 	lw	t9,0(gp)
     c20:	02202021 	move	a0,s1
     c24:	27a60020 	addiu	a2,sp,32
     c28:	0320f809 	jalr	t9
     c2c:	24050001 	li	a1,1
     c30:	8fbc0018 	lw	gp,24(sp)
     c34:	8fa60020 	lw	a2,32(sp)
     c38:	8f990000 	lw	t9,0(gp)
     c3c:	00402821 	move	a1,v0
     c40:	02202021 	move	a0,s1
     c44:	afa00010 	sw	zero,16(sp)
     c48:	0320f809 	jalr	t9
     c4c:	26470000 	addiu	a3,s2,0
     c50:	8fbc0018 	lw	gp,24(sp)
     c54:	00408021 	move	s0,v0
     c58:	8f990000 	lw	t9,0(gp)
     c5c:	27a60024 	addiu	a2,sp,36
     c60:	02202021 	move	a0,s1
     c64:	1053ffc1 	beq	v0,s3,b6c <doREPL+0x378>
     c68:	2405ffff 	li	a1,-1
     c6c:	1000ff4b 	b	99c <doREPL+0x1a8>
     c70:	00000000 	nop

00000c74 <pmain>:
     c74:	3c1c0000 	lui	gp,0x0
     c78:	279c0000 	addiu	gp,gp,0
     c7c:	0399e021 	addu	gp,gp,t9
     c80:	27bdffb0 	addiu	sp,sp,-80
     c84:	afbf004c 	sw	ra,76(sp)
     c88:	afbe0048 	sw	s8,72(sp)
     c8c:	afb70044 	sw	s7,68(sp)
     c90:	afb60040 	sw	s6,64(sp)
     c94:	afb5003c 	sw	s5,60(sp)
     c98:	afb40038 	sw	s4,56(sp)
     c9c:	afb30034 	sw	s3,52(sp)
     ca0:	afb20030 	sw	s2,48(sp)
     ca4:	afb1002c 	sw	s1,44(sp)
     ca8:	afb00028 	sw	s0,40(sp)
     cac:	afbc0018 	sw	gp,24(sp)
     cb0:	8f990000 	lw	t9,0(gp)
     cb4:	00003021 	move	a2,zero
     cb8:	24050001 	li	a1,1
     cbc:	0320f809 	jalr	t9
     cc0:	0080a021 	move	s4,a0
     cc4:	8fbc0018 	lw	gp,24(sp)
     cc8:	02802021 	move	a0,s4
     ccc:	8f990000 	lw	t9,0(gp)
     cd0:	24050002 	li	a1,2
     cd4:	0320f809 	jalr	t9
     cd8:	0040b821 	move	s7,v0
     cdc:	0040a821 	move	s5,v0
     ce0:	8c420004 	lw	v0,4(v0)
     ce4:	8fbc0018 	lw	gp,24(sp)
     ce8:	27aa0020 	addiu	t2,sp,32
     cec:	00004021 	move	t0,zero
     cf0:	10400042 	beqz	v0,dfc <pmain+0x188>
     cf4:	24060001 	li	a2,1
     cf8:	24070004 	li	a3,4
     cfc:	2409002d 	li	t1,45
     d00:	240b0069 	li	t3,105
     d04:	240d0045 	li	t5,69
     d08:	240f0065 	li	t7,101
     d0c:	240c006c 	li	t4,108
     d10:	240e0076 	li	t6,118
     d14:	1000001c 	b	d88 <pmain+0x114>
     d18:	26a50004 	addiu	a1,s5,4
     d1c:	106d00cd 	beq	v1,t5,1054 <pmain+0x3e0>
     d20:	28620046 	slti	v0,v1,70
     d24:	14400219 	bnez	v0,158c <pmain+0x918>
     d28:	00000000 	nop
     d2c:	146f0202 	bne	v1,t7,1538 <pmain+0x8c4>
     d30:	35080008 	ori	t0,t0,0x8
     d34:	00f51021 	addu	v0,a3,s5
     d38:	8c430000 	lw	v1,0(v0)
     d3c:	00000000 	nop
     d40:	80640002 	lb	a0,2(v1)
     d44:	00000000 	nop
     d48:	1480000a 	bnez	a0,d74 <pmain+0x100>
     d4c:	00000000 	nop
     d50:	24a50004 	addiu	a1,a1,4
     d54:	8ca20000 	lw	v0,0(a1)
     d58:	00000000 	nop
     d5c:	104001f3 	beqz	v0,152c <pmain+0x8b8>
     d60:	24c60001 	addiu	a2,a2,1
     d64:	80420000 	lb	v0,0(v0)
     d68:	00000000 	nop
     d6c:	104901f0 	beq	v0,t1,1530 <pmain+0x8bc>
     d70:	24190001 	li	t9,1
     d74:	24a50004 	addiu	a1,a1,4
     d78:	8ca20000 	lw	v0,0(a1)
     d7c:	24c60001 	addiu	a2,a2,1
     d80:	1040001e 	beqz	v0,dfc <pmain+0x188>
     d84:	00063880 	sll	a3,a2,0x2
     d88:	00f51021 	addu	v0,a3,s5
     d8c:	8c440000 	lw	a0,0(v0)
     d90:	ad460000 	sw	a2,0(t2)
     d94:	80820000 	lb	v0,0(a0)
     d98:	00000000 	nop
     d9c:	14490018 	bne	v0,t1,e00 <pmain+0x18c>
     da0:	00000000 	nop
     da4:	80830001 	lb	v1,1(a0)
     da8:	00000000 	nop
     dac:	106b00a7 	beq	v1,t3,104c <pmain+0x3d8>
     db0:	2862006a 	slti	v0,v1,106
     db4:	1440ffd9 	bnez	v0,d1c <pmain+0xa8>
     db8:	00000000 	nop
     dbc:	106cffde 	beq	v1,t4,d38 <pmain+0xc4>
     dc0:	00f51021 	addu	v0,a3,s5
     dc4:	146e01dd 	bne	v1,t6,153c <pmain+0x8c8>
     dc8:	24020001 	li	v0,1
     dcc:	00f51021 	addu	v0,a3,s5
     dd0:	8c430000 	lw	v1,0(v0)
     dd4:	00000000 	nop
     dd8:	80640002 	lb	a0,2(v1)
     ddc:	00000000 	nop
     de0:	148001d2 	bnez	a0,152c <pmain+0x8b8>
     de4:	24a50004 	addiu	a1,a1,4
     de8:	8ca20000 	lw	v0,0(a1)
     dec:	24c60001 	addiu	a2,a2,1
     df0:	35080004 	ori	t0,t0,0x4
     df4:	1440ffe4 	bnez	v0,d88 <pmain+0x114>
     df8:	00063880 	sll	a3,a2,0x2
     dfc:	ad460000 	sw	a2,0(t2)
     e00:	afa80024 	sw	t0,36(sp)
     e04:	8f820000 	lw	v0,0(gp)
     e08:	24030088 	li	v1,136
     e0c:	8f990000 	lw	t9,0(gp)
     e10:	8c460000 	lw	a2,0(v0)
     e14:	8c470004 	lw	a3,4(v0)
     e18:	afa30010 	sw	v1,16(sp)
     e1c:	0320f809 	jalr	t9
     e20:	02802021 	move	a0,s4
     e24:	8ea30000 	lw	v1,0(s5)
     e28:	8fbc0018 	lw	gp,24(sp)
     e2c:	10600008 	beqz	v1,e50 <pmain+0x1dc>
     e30:	00000000 	nop
     e34:	80620000 	lb	v0,0(v1)
     e38:	00000000 	nop
     e3c:	10400004 	beqz	v0,e50 <pmain+0x1dc>
     e40:	00000000 	nop
     e44:	8f820000 	lw	v0,0(gp)
     e48:	00000000 	nop
     e4c:	ac430000 	sw	v1,0(v0)
     e50:	8fa30024 	lw	v1,36(sp)
     e54:	24020001 	li	v0,1
     e58:	10620134 	beq	v1,v0,132c <pmain+0x6b8>
     e5c:	00031082 	srl	v0,v1,0x2
     e60:	30420001 	andi	v0,v0,0x1
     e64:	14400099 	bnez	v0,10cc <pmain+0x458>
     e68:	00000000 	nop
     e6c:	8fb90024 	lw	t9,36(sp)
     e70:	00000000 	nop
     e74:	00199902 	srl	s3,t9,0x4
     e78:	32620001 	andi	v0,s3,0x1
     e7c:	1440007b 	bnez	v0,106c <pmain+0x3f8>
     e80:	02802021 	move	a0,s4
     e84:	8f990000 	lw	t9,0(gp)
     e88:	00000000 	nop
     e8c:	0320f809 	jalr	t9
     e90:	02802021 	move	a0,s4
     e94:	8fb20020 	lw	s2,32(sp)
     e98:	8fbc0018 	lw	gp,24(sp)
     e9c:	12570089 	beq	s2,s7,10c4 <pmain+0x450>
     ea0:	00000000 	nop
     ea4:	8f990000 	lw	t9,0(gp)
     ea8:	02f22823 	subu	a1,s7,s2
     eac:	24a5ffff 	addiu	a1,a1,-1
     eb0:	02802021 	move	a0,s4
     eb4:	0320f809 	jalr	t9
     eb8:	26460001 	addiu	a2,s2,1
     ebc:	8fbc0018 	lw	gp,24(sp)
     ec0:	1ae00012 	blez	s7,f0c <pmain+0x298>
     ec4:	00008821 	move	s1,zero
     ec8:	02a08021 	move	s0,s5
     ecc:	8f990000 	lw	t9,0(gp)
     ed0:	8e050000 	lw	a1,0(s0)
     ed4:	0320f809 	jalr	t9
     ed8:	02802021 	move	a0,s4
     edc:	8fbc0018 	lw	gp,24(sp)
     ee0:	02323023 	subu	a2,s1,s2
     ee4:	8f990000 	lw	t9,0(gp)
     ee8:	00063fc3 	sra	a3,a2,0x1f
     eec:	26310001 	addiu	s1,s1,1
     ef0:	02802021 	move	a0,s4
     ef4:	0320f809 	jalr	t9
     ef8:	2405fffe 	li	a1,-2
     efc:	0237102a 	slt	v0,s1,s7
     f00:	8fbc0018 	lw	gp,24(sp)
     f04:	1440fff1 	bnez	v0,ecc <pmain+0x258>
     f08:	26100004 	addiu	s0,s0,4
     f0c:	8f820000 	lw	v0,0(gp)
     f10:	8f990000 	lw	t9,0(gp)
     f14:	24450000 	addiu	a1,v0,0
     f18:	0320f809 	jalr	t9
     f1c:	02802021 	move	a0,s4
     f20:	32620001 	andi	v0,s3,0x1
     f24:	8fbc0018 	lw	gp,24(sp)
     f28:	104000c4 	beqz	v0,123c <pmain+0x5c8>
     f2c:	00000000 	nop
     f30:	8fb60020 	lw	s6,32(sp)
     f34:	24130001 	li	s3,1
     f38:	0276102a 	slt	v0,s3,s6
     f3c:	1040006b 	beqz	v0,10ec <pmain+0x478>
     f40:	241e0065 	li	s8,101
     f44:	10000007 	b	f64 <pmain+0x2f0>
     f48:	26b00004 	addiu	s0,s5,4
     f4c:	1079000b 	beq	v1,t9,f7c <pmain+0x308>
     f50:	00000000 	nop
     f54:	26730001 	addiu	s3,s3,1
     f58:	0276102a 	slt	v0,s3,s6
     f5c:	10400063 	beqz	v0,10ec <pmain+0x478>
     f60:	26100004 	addiu	s0,s0,4
     f64:	8e050000 	lw	a1,0(s0)
     f68:	00000000 	nop
     f6c:	80a30001 	lb	v1,1(a1)
     f70:	00000000 	nop
     f74:	147efff5 	bne	v1,s8,f4c <pmain+0x2d8>
     f78:	2419006c 	li	t9,108
     f7c:	80a20002 	lb	v0,2(a1)
     f80:	00000000 	nop
     f84:	14400004 	bnez	v0,f98 <pmain+0x324>
     f88:	24b20002 	addiu	s2,a1,2
     f8c:	26100004 	addiu	s0,s0,4
     f90:	8e120000 	lw	s2,0(s0)
     f94:	26730001 	addiu	s3,s3,1
     f98:	107e0099 	beq	v1,s8,1200 <pmain+0x58c>
     f9c:	02402821 	move	a1,s2
     fa0:	8f820000 	lw	v0,0(gp)
     fa4:	8f990000 	lw	t9,0(gp)
     fa8:	02802021 	move	a0,s4
     fac:	0320f809 	jalr	t9
     fb0:	24450000 	addiu	a1,v0,0
     fb4:	8fbc0018 	lw	gp,24(sp)
     fb8:	02802021 	move	a0,s4
     fbc:	8f990000 	lw	t9,0(gp)
     fc0:	00000000 	nop
     fc4:	0320f809 	jalr	t9
     fc8:	02402821 	move	a1,s2
     fcc:	8fbc0018 	lw	gp,24(sp)
     fd0:	02802021 	move	a0,s4
     fd4:	8f990000 	lw	t9,0(gp)
     fd8:	24050001 	li	a1,1
     fdc:	273902f8 	addiu	t9,t9,760
     fe0:	0320f809 	jalr	t9
     fe4:	24060001 	li	a2,1
     fe8:	8fbc0018 	lw	gp,24(sp)
     fec:	1040008c 	beqz	v0,1220 <pmain+0x5ac>
     ff0:	00408821 	move	s1,v0
     ff4:	8f990000 	lw	t9,0(gp)
     ff8:	02202821 	move	a1,s1
     ffc:	2739014c 	addiu	t9,t9,332
    1000:	0320f809 	jalr	t9
    1004:	02802021 	move	a0,s4
    1008:	8fbc0018 	lw	gp,24(sp)
    100c:	1040ffd2 	beqz	v0,f58 <pmain+0x2e4>
    1010:	26730001 	addiu	s3,s3,1
    1014:	00001821 	move	v1,zero
    1018:	8fbf004c 	lw	ra,76(sp)
    101c:	8fbe0048 	lw	s8,72(sp)
    1020:	8fb70044 	lw	s7,68(sp)
    1024:	8fb60040 	lw	s6,64(sp)
    1028:	8fb5003c 	lw	s5,60(sp)
    102c:	8fb40038 	lw	s4,56(sp)
    1030:	8fb30034 	lw	s3,52(sp)
    1034:	8fb20030 	lw	s2,48(sp)
    1038:	8fb1002c 	lw	s1,44(sp)
    103c:	8fb00028 	lw	s0,40(sp)
    1040:	00601021 	move	v0,v1
    1044:	03e00008 	jr	ra
    1048:	27bd0050 	addiu	sp,sp,80
    104c:	1000ff5f 	b	dcc <pmain+0x158>
    1050:	35080002 	ori	t0,t0,0x2
    1054:	80820002 	lb	v0,2(a0)
    1058:	00000000 	nop
    105c:	14400136 	bnez	v0,1538 <pmain+0x8c4>
    1060:	35080010 	ori	t0,t0,0x10
    1064:	1000ff44 	b	d78 <pmain+0x104>
    1068:	24a50004 	addiu	a1,a1,4
    106c:	8f990000 	lw	t9,0(gp)
    1070:	00000000 	nop
    1074:	0320f809 	jalr	t9
    1078:	24050001 	li	a1,1
    107c:	8fbc0018 	lw	gp,24(sp)
    1080:	3c05fff0 	lui	a1,0xfff0
    1084:	8f860000 	lw	a2,0(gp)
    1088:	8f990000 	lw	t9,0(gp)
    108c:	02802021 	move	a0,s4
    1090:	34a5b9d8 	ori	a1,a1,0xb9d8
    1094:	0320f809 	jalr	t9
    1098:	24c60000 	addiu	a2,a2,0
    109c:	8fbc0018 	lw	gp,24(sp)
    10a0:	00000000 	nop
    10a4:	8f990000 	lw	t9,0(gp)
    10a8:	00000000 	nop
    10ac:	0320f809 	jalr	t9
    10b0:	02802021 	move	a0,s4
    10b4:	8fb20020 	lw	s2,32(sp)
    10b8:	8fbc0018 	lw	gp,24(sp)
    10bc:	1657ff79 	bne	s2,s7,ea4 <pmain+0x230>
    10c0:	00000000 	nop
    10c4:	1000ff77 	b	ea4 <pmain+0x230>
    10c8:	00009021 	move	s2,zero
    10cc:	8f990000 	lw	t9,0(gp)
    10d0:	00000000 	nop
    10d4:	2739041c 	addiu	t9,t9,1052
    10d8:	0320f809 	jalr	t9
    10dc:	00000000 	nop
    10e0:	8fbc0018 	lw	gp,24(sp)
    10e4:	1000ff61 	b	e6c <pmain+0x1f8>
    10e8:	00000000 	nop
    10ec:	8fa30020 	lw	v1,32(sp)
    10f0:	00000000 	nop
    10f4:	0077102a 	slt	v0,v1,s7
    10f8:	10400024 	beqz	v0,118c <pmain+0x518>
    10fc:	00031080 	sll	v0,v1,0x2
    1100:	00558821 	addu	s1,v0,s5
    1104:	8f850000 	lw	a1,0(gp)
    1108:	8e300000 	lw	s0,0(s1)
    110c:	8f990000 	lw	t9,0(gp)
    1110:	24a50000 	addiu	a1,a1,0
    1114:	0320f809 	jalr	t9
    1118:	02002021 	move	a0,s0
    111c:	8fbc0018 	lw	gp,24(sp)
    1120:	1440000a 	bnez	v0,114c <pmain+0x4d8>
    1124:	00000000 	nop
    1128:	8f850000 	lw	a1,0(gp)
    112c:	8f990000 	lw	t9,0(gp)
    1130:	8e24fffc 	lw	a0,-4(s1)
    1134:	0320f809 	jalr	t9
    1138:	24a50000 	addiu	a1,a1,0
    113c:	8fbc0018 	lw	gp,24(sp)
    1140:	10400002 	beqz	v0,114c <pmain+0x4d8>
    1144:	00000000 	nop
    1148:	00008021 	move	s0,zero
    114c:	8f990000 	lw	t9,0(gp)
    1150:	02002821 	move	a1,s0
    1154:	02802021 	move	a0,s4
    1158:	0320f809 	jalr	t9
    115c:	00003021 	move	a2,zero
    1160:	8fbc0018 	lw	gp,24(sp)
    1164:	104000a6 	beqz	v0,1400 <pmain+0x78c>
    1168:	00000000 	nop
    116c:	8f990000 	lw	t9,0(gp)
    1170:	00402821 	move	a1,v0
    1174:	2739014c 	addiu	t9,t9,332
    1178:	0320f809 	jalr	t9
    117c:	02802021 	move	a0,s4
    1180:	8fbc0018 	lw	gp,24(sp)
    1184:	14400011 	bnez	v0,11cc <pmain+0x558>
    1188:	00001821 	move	v1,zero
    118c:	8fa30024 	lw	v1,36(sp)
    1190:	00000000 	nop
    1194:	00031042 	srl	v0,v1,0x1
    1198:	30420001 	andi	v0,v0,0x1
    119c:	1440005b 	bnez	v0,130c <pmain+0x698>
    11a0:	00000000 	nop
    11a4:	8fa20020 	lw	v0,32(sp)
    11a8:	00000000 	nop
    11ac:	10570044 	beq	v0,s7,12c0 <pmain+0x64c>
    11b0:	00000000 	nop
    11b4:	8f990000 	lw	t9,0(gp)
    11b8:	02802021 	move	a0,s4
    11bc:	0320f809 	jalr	t9
    11c0:	24050001 	li	a1,1
    11c4:	8fbc0018 	lw	gp,24(sp)
    11c8:	24030001 	li	v1,1
    11cc:	8fbf004c 	lw	ra,76(sp)
    11d0:	8fbe0048 	lw	s8,72(sp)
    11d4:	8fb70044 	lw	s7,68(sp)
    11d8:	8fb60040 	lw	s6,64(sp)
    11dc:	8fb5003c 	lw	s5,60(sp)
    11e0:	8fb40038 	lw	s4,56(sp)
    11e4:	8fb30034 	lw	s3,52(sp)
    11e8:	8fb20030 	lw	s2,48(sp)
    11ec:	8fb1002c 	lw	s1,44(sp)
    11f0:	8fb00028 	lw	s0,40(sp)
    11f4:	00601021 	move	v0,v1
    11f8:	03e00008 	jr	ra
    11fc:	27bd0050 	addiu	sp,sp,80
    1200:	8f820000 	lw	v0,0(gp)
    1204:	8f990000 	lw	t9,0(gp)
    1208:	02802021 	move	a0,s4
    120c:	27390568 	addiu	t9,t9,1384
    1210:	0320f809 	jalr	t9
    1214:	24460000 	addiu	a2,v0,0
    1218:	1000ff7b 	b	1008 <pmain+0x394>
    121c:	00000000 	nop
    1220:	8f990000 	lw	t9,0(gp)
    1224:	02402821 	move	a1,s2
    1228:	0320f809 	jalr	t9
    122c:	02802021 	move	a0,s4
    1230:	8fbc0018 	lw	gp,24(sp)
    1234:	1000ff6f 	b	ff4 <pmain+0x380>
    1238:	00000000 	nop
    123c:	8f820000 	lw	v0,0(gp)
    1240:	8f990000 	lw	t9,0(gp)
    1244:	24500000 	addiu	s0,v0,0
    1248:	0320f809 	jalr	t9
    124c:	26040001 	addiu	a0,s0,1
    1250:	8fbc0018 	lw	gp,24(sp)
    1254:	104000bb 	beqz	v0,1544 <pmain+0x8d0>
    1258:	00402821 	move	a1,v0
    125c:	80a30000 	lb	v1,0(a1)
    1260:	24020040 	li	v0,64
    1264:	106200a5 	beq	v1,v0,14fc <pmain+0x888>
    1268:	02003021 	move	a2,s0
    126c:	8f990000 	lw	t9,0(gp)
    1270:	00000000 	nop
    1274:	27390568 	addiu	t9,t9,1384
    1278:	0320f809 	jalr	t9
    127c:	02802021 	move	a0,s4
    1280:	8fbc0018 	lw	gp,24(sp)
    1284:	1040ff2a 	beqz	v0,f30 <pmain+0x2bc>
    1288:	00001821 	move	v1,zero
    128c:	8fbf004c 	lw	ra,76(sp)
    1290:	8fbe0048 	lw	s8,72(sp)
    1294:	8fb70044 	lw	s7,68(sp)
    1298:	8fb60040 	lw	s6,64(sp)
    129c:	8fb5003c 	lw	s5,60(sp)
    12a0:	8fb40038 	lw	s4,56(sp)
    12a4:	8fb30034 	lw	s3,52(sp)
    12a8:	8fb20030 	lw	s2,48(sp)
    12ac:	8fb1002c 	lw	s1,44(sp)
    12b0:	8fb00028 	lw	s0,40(sp)
    12b4:	00601021 	move	v0,v1
    12b8:	03e00008 	jr	ra
    12bc:	27bd0050 	addiu	sp,sp,80
    12c0:	8fb90024 	lw	t9,36(sp)
    12c4:	00000000 	nop
    12c8:	3322000c 	andi	v0,t9,0xc
    12cc:	1440ffb9 	bnez	v0,11b4 <pmain+0x540>
    12d0:	00000000 	nop
    12d4:	8f990000 	lw	t9,0(gp)
    12d8:	00000000 	nop
    12dc:	0320f809 	jalr	t9
    12e0:	00002021 	move	a0,zero
    12e4:	8fbc0018 	lw	gp,24(sp)
    12e8:	104000a0 	beqz	v0,156c <pmain+0x8f8>
    12ec:	02802021 	move	a0,s4
    12f0:	8f990000 	lw	t9,0(gp)
    12f4:	00000000 	nop
    12f8:	2739041c 	addiu	t9,t9,1052
    12fc:	0320f809 	jalr	t9
    1300:	00000000 	nop
    1304:	8fbc0018 	lw	gp,24(sp)
    1308:	00000000 	nop
    130c:	8f990000 	lw	t9,0(gp)
    1310:	00000000 	nop
    1314:	273907f4 	addiu	t9,t9,2036
    1318:	0320f809 	jalr	t9
    131c:	02802021 	move	a0,s4
    1320:	8fbc0018 	lw	gp,24(sp)
    1324:	1000ffa3 	b	11b4 <pmain+0x540>
    1328:	00000000 	nop
    132c:	8f910000 	lw	s1,0(gp)
    1330:	8f920000 	lw	s2,0(gp)
    1334:	8fa20020 	lw	v0,32(sp)
    1338:	8f850000 	lw	a1,0(gp)
    133c:	8f990000 	lw	t9,0(gp)
    1340:	8e240000 	lw	a0,0(s1)
    1344:	8e460000 	lw	a2,0(s2)
    1348:	00021080 	sll	v0,v0,0x2
    134c:	00551021 	addu	v0,v0,s5
    1350:	8c500000 	lw	s0,0(v0)
    1354:	0320f809 	jalr	t9
    1358:	24a50000 	addiu	a1,a1,0
    135c:	8fbc0018 	lw	gp,24(sp)
    1360:	8e240000 	lw	a0,0(s1)
    1364:	8f990000 	lw	t9,0(gp)
    1368:	00000000 	nop
    136c:	0320f809 	jalr	t9
    1370:	00000000 	nop
    1374:	82030001 	lb	v1,1(s0)
    1378:	24020065 	li	v0,101
    137c:	8fbc0018 	lw	gp,24(sp)
    1380:	10620003 	beq	v1,v0,1390 <pmain+0x71c>
    1384:	2402006c 	li	v0,108
    1388:	14620063 	bne	v1,v0,1518 <pmain+0x8a4>
    138c:	00000000 	nop
    1390:	8f850000 	lw	a1,0(gp)
    1394:	8f990000 	lw	t9,0(gp)
    1398:	8e240000 	lw	a0,0(s1)
    139c:	24a50000 	addiu	a1,a1,0
    13a0:	0320f809 	jalr	t9
    13a4:	02003021 	move	a2,s0
    13a8:	8fbc0018 	lw	gp,24(sp)
    13ac:	8e240000 	lw	a0,0(s1)
    13b0:	8f990000 	lw	t9,0(gp)
    13b4:	00000000 	nop
    13b8:	0320f809 	jalr	t9
    13bc:	00000000 	nop
    13c0:	8fbc0018 	lw	gp,24(sp)
    13c4:	8e460000 	lw	a2,0(s2)
    13c8:	8f850000 	lw	a1,0(gp)
    13cc:	8f990000 	lw	t9,0(gp)
    13d0:	8e240000 	lw	a0,0(s1)
    13d4:	0320f809 	jalr	t9
    13d8:	24a50000 	addiu	a1,a1,0
    13dc:	8fbc0018 	lw	gp,24(sp)
    13e0:	8e240000 	lw	a0,0(s1)
    13e4:	8f990000 	lw	t9,0(gp)
    13e8:	00000000 	nop
    13ec:	0320f809 	jalr	t9
    13f0:	00000000 	nop
    13f4:	8fbc0018 	lw	gp,24(sp)
    13f8:	1000ff07 	b	1018 <pmain+0x3a4>
    13fc:	00001821 	move	v1,zero
    1400:	8f820000 	lw	v0,0(gp)
    1404:	8f990000 	lw	t9,0(gp)
    1408:	24450000 	addiu	a1,v0,0
    140c:	0320f809 	jalr	t9
    1410:	02802021 	move	a0,s4
    1414:	24030005 	li	v1,5
    1418:	8fbc0018 	lw	gp,24(sp)
    141c:	10430008 	beq	v0,v1,1440 <pmain+0x7cc>
    1420:	00000000 	nop
    1424:	8f850000 	lw	a1,0(gp)
    1428:	8f990000 	lw	t9,0(gp)
    142c:	24a50000 	addiu	a1,a1,0
    1430:	0320f809 	jalr	t9
    1434:	02802021 	move	a0,s4
    1438:	8fbc0018 	lw	gp,24(sp)
    143c:	00000000 	nop
    1440:	8f990000 	lw	t9,0(gp)
    1444:	02802021 	move	a0,s4
    1448:	0320f809 	jalr	t9
    144c:	2405ffff 	li	a1,-1
    1450:	8fbc0018 	lw	gp,24(sp)
    1454:	00408821 	move	s1,v0
    1458:	8f860000 	lw	a2,0(gp)
    145c:	8f990000 	lw	t9,0(gp)
    1460:	02802021 	move	a0,s4
    1464:	24c60000 	addiu	a2,a2,0
    1468:	0320f809 	jalr	t9
    146c:	24450003 	addiu	a1,v0,3
    1470:	8fbc0018 	lw	gp,24(sp)
    1474:	1a20000c 	blez	s1,14a8 <pmain+0x834>
    1478:	24100001 	li	s0,1
    147c:	8f990000 	lw	t9,0(gp)
    1480:	02003021 	move	a2,s0
    1484:	00103fc3 	sra	a3,s0,0x1f
    1488:	00102823 	negu	a1,s0
    148c:	02802021 	move	a0,s4
    1490:	0320f809 	jalr	t9
    1494:	26100001 	addiu	s0,s0,1
    1498:	0230182a 	slt	v1,s1,s0
    149c:	8fbc0018 	lw	gp,24(sp)
    14a0:	1060fff6 	beqz	v1,147c <pmain+0x808>
    14a4:	00000000 	nop
    14a8:	8f990000 	lw	t9,0(gp)
    14ac:	2406ffff 	li	a2,-1
    14b0:	00102823 	negu	a1,s0
    14b4:	0320f809 	jalr	t9
    14b8:	02802021 	move	a0,s4
    14bc:	8fbc0018 	lw	gp,24(sp)
    14c0:	02802021 	move	a0,s4
    14c4:	8f990000 	lw	t9,0(gp)
    14c8:	00000000 	nop
    14cc:	0320f809 	jalr	t9
    14d0:	2405fffe 	li	a1,-2
    14d4:	8fbc0018 	lw	gp,24(sp)
    14d8:	02802021 	move	a0,s4
    14dc:	8f990000 	lw	t9,0(gp)
    14e0:	02202821 	move	a1,s1
    14e4:	273902f8 	addiu	t9,t9,760
    14e8:	0320f809 	jalr	t9
    14ec:	2406ffff 	li	a2,-1
    14f0:	8fbc0018 	lw	gp,24(sp)
    14f4:	1000ff1d 	b	116c <pmain+0x4f8>
    14f8:	00000000 	nop
    14fc:	8f990000 	lw	t9,0(gp)
    1500:	24a50001 	addiu	a1,a1,1
    1504:	27390518 	addiu	t9,t9,1304
    1508:	0320f809 	jalr	t9
    150c:	02802021 	move	a0,s4
    1510:	1000ff5b 	b	1280 <pmain+0x60c>
    1514:	00000000 	nop
    1518:	8f850000 	lw	a1,0(gp)
    151c:	8f990000 	lw	t9,0(gp)
    1520:	8e240000 	lw	a0,0(s1)
    1524:	1000ff9e 	b	13a0 <pmain+0x72c>
    1528:	24a50000 	addiu	a1,a1,0
    152c:	24190001 	li	t9,1
    1530:	1000fe34 	b	e04 <pmain+0x190>
    1534:	afb90024 	sw	t9,36(sp)
    1538:	24020001 	li	v0,1
    153c:	1000fe31 	b	e04 <pmain+0x190>
    1540:	afa20024 	sw	v0,36(sp)
    1544:	8f820000 	lw	v0,0(gp)
    1548:	8f990000 	lw	t9,0(gp)
    154c:	24500000 	addiu	s0,v0,0
    1550:	0320f809 	jalr	t9
    1554:	26040001 	addiu	a0,s0,1
    1558:	8fbc0018 	lw	gp,24(sp)
    155c:	1440ff3f 	bnez	v0,125c <pmain+0x5e8>
    1560:	00402821 	move	a1,v0
    1564:	1000fe72 	b	f30 <pmain+0x2bc>
    1568:	00000000 	nop
    156c:	8f990000 	lw	t9,0(gp)
    1570:	00000000 	nop
    1574:	27390518 	addiu	t9,t9,1304
    1578:	0320f809 	jalr	t9
    157c:	00002821 	move	a1,zero
    1580:	8fbc0018 	lw	gp,24(sp)
    1584:	1000ff0b 	b	11b4 <pmain+0x540>
    1588:	00000000 	nop
    158c:	1060fe1d 	beqz	v1,e04 <pmain+0x190>
    1590:	afa80024 	sw	t0,36(sp)
    1594:	1469ffe9 	bne	v1,t1,153c <pmain+0x8c8>
    1598:	24020001 	li	v0,1
    159c:	80820002 	lb	v0,2(a0)
    15a0:	24030001 	li	v1,1
    15a4:	1440fe17 	bnez	v0,e04 <pmain+0x190>
    15a8:	afa30024 	sw	v1,36(sp)
    15ac:	24c20001 	addiu	v0,a2,1
    15b0:	afa80024 	sw	t0,36(sp)
    15b4:	1000fe13 	b	e04 <pmain+0x190>
    15b8:	ad420000 	sw	v0,0(t2)

000015bc <main>:
    15bc:	3c1c0000 	lui	gp,0x0
    15c0:	279c0000 	addiu	gp,gp,0
    15c4:	0399e021 	addu	gp,gp,t9
    15c8:	27bdffc8 	addiu	sp,sp,-56
    15cc:	afbf0034 	sw	ra,52(sp)
    15d0:	afb40030 	sw	s4,48(sp)
    15d4:	afb3002c 	sw	s3,44(sp)
    15d8:	afb20028 	sw	s2,40(sp)
    15dc:	afb10024 	sw	s1,36(sp)
    15e0:	afb00020 	sw	s0,32(sp)
    15e4:	afbc0018 	sw	gp,24(sp)
    15e8:	8f990000 	lw	t9,0(gp)
    15ec:	8f930000 	lw	s3,0(gp)
    15f0:	8f940000 	lw	s4,0(gp)
    15f4:	00808021 	move	s0,a0
    15f8:	0320f809 	jalr	t9
    15fc:	00a09021 	move	s2,a1
    1600:	8fbc0018 	lw	gp,24(sp)
    1604:	00408821 	move	s1,v0
    1608:	8f850000 	lw	a1,0(gp)
    160c:	8f990000 	lw	t9,0(gp)
    1610:	24a50c74 	addiu	a1,a1,3188
    1614:	00402021 	move	a0,v0
    1618:	10400044 	beqz	v0,172c <main+0x170>
    161c:	00003021 	move	a2,zero
    1620:	0320f809 	jalr	t9
    1624:	00000000 	nop
    1628:	8fbc0018 	lw	gp,24(sp)
    162c:	02003021 	move	a2,s0
    1630:	8f990000 	lw	t9,0(gp)
    1634:	00103fc3 	sra	a3,s0,0x1f
    1638:	0320f809 	jalr	t9
    163c:	02202021 	move	a0,s1
    1640:	8fbc0018 	lw	gp,24(sp)
    1644:	02202021 	move	a0,s1
    1648:	8f990000 	lw	t9,0(gp)
    164c:	00000000 	nop
    1650:	0320f809 	jalr	t9
    1654:	02402821 	move	a1,s2
    1658:	8fbc0018 	lw	gp,24(sp)
    165c:	00003821 	move	a3,zero
    1660:	8f990000 	lw	t9,0(gp)
    1664:	24060001 	li	a2,1
    1668:	02202021 	move	a0,s1
    166c:	afa00010 	sw	zero,16(sp)
    1670:	afa00014 	sw	zero,20(sp)
    1674:	0320f809 	jalr	t9
    1678:	24050002 	li	a1,2
    167c:	8fbc0018 	lw	gp,24(sp)
    1680:	02202021 	move	a0,s1
    1684:	8f990000 	lw	t9,0(gp)
    1688:	2405ffff 	li	a1,-1
    168c:	0320f809 	jalr	t9
    1690:	00408021 	move	s0,v0
    1694:	8fbc0018 	lw	gp,24(sp)
    1698:	02202021 	move	a0,s1
    169c:	8f990000 	lw	t9,0(gp)
    16a0:	00000000 	nop
    16a4:	2739014c 	addiu	t9,t9,332
    16a8:	0320f809 	jalr	t9
    16ac:	02002821 	move	a1,s0
    16b0:	8fbc0018 	lw	gp,24(sp)
    16b4:	26850000 	addiu	a1,s4,0
    16b8:	8f990000 	lw	t9,0(gp)
    16bc:	24060261 	li	a2,609
    16c0:	0320f809 	jalr	t9
    16c4:	26640000 	addiu	a0,s3,0
    16c8:	8fbc0018 	lw	gp,24(sp)
    16cc:	00000000 	nop
    16d0:	8f990000 	lw	t9,0(gp)
    16d4:	00000000 	nop
    16d8:	0320f809 	jalr	t9
    16dc:	02202021 	move	a0,s1
    16e0:	8fbc0018 	lw	gp,24(sp)
    16e4:	26640000 	addiu	a0,s3,0
    16e8:	8f990000 	lw	t9,0(gp)
    16ec:	26850000 	addiu	a1,s4,0
    16f0:	0320f809 	jalr	t9
    16f4:	24060263 	li	a2,611
    16f8:	8fbc0018 	lw	gp,24(sp)
    16fc:	26640000 	addiu	a0,s3,0
    1700:	8f990000 	lw	t9,0(gp)
    1704:	26850000 	addiu	a1,s4,0
    1708:	8fbf0034 	lw	ra,52(sp)
    170c:	8fb40030 	lw	s4,48(sp)
    1710:	8fb3002c 	lw	s3,44(sp)
    1714:	8fb20028 	lw	s2,40(sp)
    1718:	8fb10024 	lw	s1,36(sp)
    171c:	8fb00020 	lw	s0,32(sp)
    1720:	24060267 	li	a2,615
    1724:	03200008 	jr	t9
    1728:	27bd0038 	addiu	sp,sp,56
    172c:	8f850000 	lw	a1,0(gp)
    1730:	8f990000 	lw	t9,0(gp)
    1734:	8e440000 	lw	a0,0(s2)
    1738:	8fbf0034 	lw	ra,52(sp)
    173c:	8fb40030 	lw	s4,48(sp)
    1740:	8fb3002c 	lw	s3,44(sp)
    1744:	8fb20028 	lw	s2,40(sp)
    1748:	8fb10024 	lw	s1,36(sp)
    174c:	8fb00020 	lw	s0,32(sp)
    1750:	24a50000 	addiu	a1,a1,0
    1754:	273900ac 	addiu	t9,t9,172
    1758:	03200008 	jr	t9
    175c:	27bd0038 	addiu	sp,sp,56
Disassembly of section .bss:

00000000 <globalL>:
	...
Disassembly of section .reginfo:

00000000 <.reginfo>:
   0:	f2fffffc 	0xf2fffffc
	...
Disassembly of section .pdr:

00000000 <.pdr>:
   0:	00000000 	nop
   4:	80010000 	lb	at,0(zero)
   8:	fffffffc 	0xfffffffc
	...
  14:	00000020 	add	zero,zero,zero
  18:	0000001d 	0x1d
  1c:	0000001f 	0x1f
  20:	00000058 	0x58
  24:	80000000 	lb	zero,0(zero)
  28:	fffffff8 	0xfffffff8
	...
  34:	00000020 	add	zero,zero,zero
  38:	0000001d 	0x1d
  3c:	0000001f 	0x1f
  40:	000000ac 	0xac
  44:	80030000 	lb	v1,0(zero)
  48:	fffffff8 	0xfffffff8
	...
  54:	00000028 	0x28
  58:	0000001d 	0x1d
  5c:	0000001f 	0x1f
  60:	0000014c 	syscall	0x5
  64:	80030000 	lb	v1,0(zero)
  68:	fffffff8 	0xfffffff8
	...
  74:	00000028 	0x28
  78:	0000001d 	0x1d
  7c:	0000001f 	0x1f
  80:	000001f8 	0x1f8
  84:	80010000 	lb	at,0(zero)
  88:	fffffffc 	0xfffffffc
	...
  94:	00000020 	add	zero,zero,zero
  98:	0000001d 	0x1d
  9c:	0000001f 	0x1f
  a0:	000002f8 	0x2f8
  a4:	800f0000 	lb	t7,0(zero)
  a8:	fffffff8 	0xfffffff8
	...
  b4:	00000038 	0x38
  b8:	0000001d 	0x1d
  bc:	0000001f 	0x1f
  c0:	0000041c 	0x41c
  c4:	80010000 	lb	at,0(zero)
  c8:	fffffffc 	0xfffffffc
	...
  d4:	00000020 	add	zero,zero,zero
  d8:	0000001d 	0x1d
  dc:	0000001f 	0x1f
  e0:	00000494 	0x494
  e4:	80010000 	lb	at,0(zero)
  e8:	fffffffc 	0xfffffffc
	...
  f4:	00000020 	add	zero,zero,zero
  f8:	0000001d 	0x1d
  fc:	0000001f 	0x1f
 100:	00000518 	0x518
 104:	80010000 	lb	at,0(zero)
 108:	fffffffc 	0xfffffffc
	...
 114:	00000020 	add	zero,zero,zero
 118:	0000001d 	0x1d
 11c:	0000001f 	0x1f
 120:	00000568 	0x568
 124:	80070000 	lb	a3,0(zero)
 128:	fffffffc 	0xfffffffc
	...
 134:	00000030 	0x30
 138:	0000001d 	0x1d
 13c:	0000001f 	0x1f
 140:	000005f0 	0x5f0
 144:	800f0000 	lb	t7,0(zero)
 148:	fffffff8 	0xfffffff8
	...
 154:	00000230 	0x230
 158:	0000001d 	0x1d
 15c:	0000001f 	0x1f
 160:	000007f4 	0x7f4
 164:	c0ff0000 	lwc0	$31,0(a3)
 168:	fffffffc 	0xfffffffc
	...
 174:	00000058 	0x58
 178:	0000001d 	0x1d
 17c:	0000001f 	0x1f
 180:	00000c74 	0xc74
 184:	c0ff0000 	lwc0	$31,0(a3)
 188:	fffffffc 	0xfffffffc
	...
 194:	00000050 	0x50
 198:	0000001d 	0x1d
 19c:	0000001f 	0x1f
 1a0:	00000000 	nop
 1a4:	801f0000 	lb	ra,0(zero)
 1a8:	fffffffc 	0xfffffffc
	...
 1b4:	00000038 	0x38
 1b8:	0000001d 	0x1d
 1bc:	0000001f 	0x1f
Disassembly of section .rodata.str1.4:

00000000 <$LC1-0x4>:
   0:	0061756c 	0x61756c

00000004 <$LC1>:
   4:	65746e69 	0x65746e69
   8:	70757272 	0x70757272
   c:	21646574 	addi	a0,t3,25972
  10:	00000000 	nop

00000014 <$LC2>:
  14:	203a7325 	addi	k0,at,29477
  18:	00000000 	nop

0000001c <$LC3>:
  1c:	000a7325 	0xa7325

00000020 <$LC4>:
  20:	6f745f5f 	0x6f745f5f
  24:	69727473 	0x69727473
  28:	0000676e 	0x676e

0000002c <$LC5>:
  2c:	72726528 	0x72726528
  30:	6f20726f 	0x6f20726f
  34:	63656a62 	0x63656a62
  38:	73692074 	0x73692074
  3c:	25206120 	addiu	zero,t1,24864
  40:	61762073 	0x61762073
  44:	2965756c 	slti	a1,t3,30060
  48:	00000000 	nop

0000004c <$LC6>:
  4c:	2061754c 	addi	at,v1,30028
  50:	2e332e35 	sltiu	s3,s1,11829
  54:	43202034 	c0	0x1202034
  58:	7279706f 	0x7279706f
  5c:	74686769 	jalx	1a19da4 <main+0x1a187e8>
  60:	29432820 	slti	v1,t2,10272
  64:	39393120 	xori	t9,t1,0x3120
  68:	30322d34 	andi	s2,at,0x2d34
  6c:	4c203731 	0x4c203731
  70:	6f2e6175 	0x6f2e6175
  74:	202c6772 	addi	t4,at,26482
  78:	2d435550 	sltiu	v1,t2,21840
  7c:	006f6952 	0x6f6952

00000080 <$LC7>:
  80:	0000000a 	0xa

00000084 <$LC8>:
  84:	4f52505f 	c3	0x152505f
  88:	0054504d 	break	0x54,0x141

0000008c <$LC9>:
  8c:	4f52505f 	c3	0x152505f
  90:	3254504d 	andi	s4,s2,0x504d
  94:	00000000 	nop

00000098 <$LC10>:
  98:	0000203e 	0x203e

0000009c <$LC11>:
  9c:	00203e3e 	0x203e3e

000000a0 <$LC12>:
  a0:	75746572 	jalx	5d195c8 <main+0x5d1800c>
  a4:	25206e72 	addiu	zero,t1,28274
  a8:	00000073 	0x73

000000ac <$LC13>:
  ac:	75746572 	jalx	5d195c8 <main+0x5d1800c>
  b0:	25206e72 	addiu	zero,t1,28274
  b4:	00003b73 	0x3b73

000000b8 <$LC14>:
  b8:	6474733d 	0x6474733d
  bc:	00006e69 	0x6e69

000000c0 <$LC16>:
  c0:	206f6f74 	addi	t7,v1,28532
  c4:	796e616d 	0x796e616d
  c8:	73657220 	0x73657220
  cc:	73746c75 	0x73746c75
  d0:	206f7420 	addi	t7,v1,29728
  d4:	6e697270 	0x6e697270
  d8:	00000074 	0x74

000000dc <$LC17>:
  dc:	6e697270 	0x6e697270
  e0:	00000074 	0x74

000000e4 <$LC18>:
  e4:	6f727265 	0x6f727265
  e8:	61632072 	0x61632072
  ec:	6e696c6c 	0x6e696c6c
  f0:	70272067 	0x70272067
  f4:	746e6972 	jalx	1b9a5c8 <main+0x1b9900c>
  f8:	25282027 	addiu	t0,t1,8231
  fc:	00002973 	0x2973

00000100 <$LC15>:
 100:	666f653c 	0x666f653c
 104:	0000003e 	0x3e

00000108 <$LC24>:
 108:	00677261 	0x677261

0000010c <$LC28>:
 10c:	75716572 	jalx	5c595c8 <main+0x5c5800c>
 110:	00657269 	0x657269

00000114 <$LC23>:
 114:	5f41554c 	0x5f41554c
 118:	4e454f4e 	c3	0x454f4e
 11c:	00000056 	0x56

00000120 <$LC29>:
 120:	0000002d 	0x2d

00000124 <$LC30>:
 124:	00002d2d 	0x2d2d

00000128 <$LC27>:
 128:	6f63283d 	0x6f63283d
 12c:	6e616d6d 	0x6e616d6d
 130:	696c2064 	0x696c2064
 134:	0029656e 	0x29656e

00000138 <$LC25>:
 138:	41554c3d 	0x41554c3d
 13c:	494e495f 	0x494e495f
 140:	5f355f54 	0x5f355f54
 144:	00000033 	0x33

00000148 <$LC20>:
 148:	27732527 	addiu	s3,k1,9511
 14c:	65656e20 	0x65656e20
 150:	61207364 	0x61207364
 154:	6d756772 	0x6d756772
 158:	0a746e65 	j	9d1b994 <main+0x9d1a3d8>
 15c:	00000000 	nop

00000160 <$LC22>:
 160:	67617375 	0x67617375
 164:	25203a65 	addiu	zero,t1,14949
 168:	6f5b2073 	0x6f5b2073
 16c:	6f697470 	0x6f697470
 170:	205d736e 	addi	sp,v0,29550
 174:	7263735b 	0x7263735b
 178:	20747069 	addi	s4,v1,28777
 17c:	6772615b 	0x6772615b
 180:	0a5d5d73 	j	97575cc <main+0x9756010>
 184:	69617641 	0x69617641
 188:	6c62616c 	0x6c62616c
 18c:	706f2065 	0x706f2065
 190:	6e6f6974 	0x6e6f6974
 194:	72612073 	0x72612073
 198:	200a3a65 	addi	t2,zero,14949
 19c:	20652d20 	addi	a1,v1,11552
 1a0:	74617473 	jalx	185d1cc <main+0x185bc10>
 1a4:	78652020 	0x78652020
 1a8:	74756365 	jalx	1d58d94 <main+0x1d577d8>
 1ac:	74732065 	jalx	1cc8194 <main+0x1cc6bd8>
 1b0:	676e6972 	0x676e6972
 1b4:	74732720 	jalx	1cc9c80 <main+0x1cc86c4>
 1b8:	0a277461 	j	89dd184 <main+0x89dbbc8>
 1bc:	692d2020 	0x692d2020
 1c0:	20202020 	addi	zero,at,8224
 1c4:	65202020 	0x65202020
 1c8:	7265746e 	0x7265746e
 1cc:	746e6920 	jalx	1b9a480 <main+0x1b98ec4>
 1d0:	63617265 	0x63617265
 1d4:	65766974 	0x65766974
 1d8:	646f6d20 	0x646f6d20
 1dc:	66612065 	0x66612065
 1e0:	20726574 	addi	s2,v1,25972
 1e4:	63657865 	0x63657865
 1e8:	6e697475 	0x6e697475
 1ec:	73272067 	0x73272067
 1f0:	70697263 	0x70697263
 1f4:	200a2774 	addi	t2,zero,10100
 1f8:	206c2d20 	addi	t4,v1,11552
 1fc:	656d616e 	0x656d616e
 200:	65722020 	0x65722020
 204:	72697571 	0x72697571
 208:	696c2065 	0x696c2065
 20c:	72617262 	0x72617262
 210:	6e272079 	0x6e272079
 214:	27656d61 	addiu	a1,k1,28001
 218:	2d20200a 	sltiu	zero,t1,8202
 21c:	20202076 	addi	zero,at,8310
 220:	20202020 	addi	zero,at,8224
 224:	776f6873 	jalx	dbda1cc <main+0xdbd8c10>
 228:	72657620 	0x72657620
 22c:	6e6f6973 	0x6e6f6973
 230:	666e6920 	0x666e6920
 234:	616d726f 	0x616d726f
 238:	6e6f6974 	0x6e6f6974
 23c:	2d20200a 	sltiu	zero,t1,8202
 240:	20202045 	addi	zero,at,8261
 244:	20202020 	addi	zero,at,8224
 248:	6f6e6769 	0x6f6e6769
 24c:	65206572 	0x65206572
 250:	7269766e 	0x7269766e
 254:	656d6e6f 	0x656d6e6f
 258:	7620746e 	jalx	881d1b8 <main+0x881bbfc>
 25c:	61697261 	0x61697261
 260:	73656c62 	0x73656c62
 264:	2d20200a 	sltiu	zero,t1,8202
 268:	2020202d 	addi	zero,at,8237
 26c:	20202020 	addi	zero,at,8224
 270:	706f7473 	0x706f7473
 274:	6e616820 	0x6e616820
 278:	6e696c64 	0x6e696c64
 27c:	706f2067 	0x706f2067
 280:	6e6f6974 	0x6e6f6974
 284:	20200a73 	addi	zero,at,2675
 288:	2020202d 	addi	zero,at,8237
 28c:	20202020 	addi	zero,at,8224
 290:	6f747320 	0x6f747320
 294:	61682070 	0x61682070
 298:	696c646e 	0x696c646e
 29c:	6f20676e 	0x6f20676e
 2a0:	6f697470 	0x6f697470
 2a4:	6120736e 	0x6120736e
 2a8:	6520646e 	0x6520646e
 2ac:	75636578 	jalx	58d95e0 <main+0x58d8024>
 2b0:	73206574 	0x73206574
 2b4:	6e696474 	0x6e696474
 2b8:	0000000a 	0xa

000002bc <$LC31>:
 2bc:	67726127 	0x67726127
 2c0:	73692027 	0x73692027
 2c4:	746f6e20 	jalx	1bdb880 <main+0x1bda2c4>
 2c8:	74206120 	jalx	818480 <main+0x816ec4>
 2cc:	656c6261 	0x656c6261
 2d0:	00000000 	nop

000002d4 <$LC32>:
 2d4:	206f6f74 	addi	t7,v1,28532
 2d8:	796e616d 	0x796e616d
 2dc:	67726120 	0x67726120
 2e0:	6e656d75 	0x6e656d75
 2e4:	74207374 	jalx	81cdd0 <main+0x81b814>
 2e8:	6373206f 	0x6373206f
 2ec:	74706972 	jalx	1c1a5c8 <main+0x1c1900c>
 2f0:	00000000 	nop

000002f4 <$LC21>:
 2f4:	65726e75 	0x65726e75
 2f8:	6e676f63 	0x6e676f63
 2fc:	64657a69 	0x64657a69
 300:	74706f20 	jalx	1c1bc80 <main+0x1c1a6c4>
 304:	206e6f69 	addi	t6,v1,28521
 308:	27732527 	addiu	s3,k1,9511
 30c:	0000000a 	0xa

00000310 <$LC26>:
 310:	41554c3d 	0x41554c3d
 314:	494e495f 	0x494e495f
 318:	00000054 	0x54

0000031c <$LC34>:
 31c:	25207325 	addiu	zero,t1,29477
 320:	00000a64 	0xa64

00000324 <$LC35>:
 324:	2e61756c 	sltiu	at,s3,30060
 328:	00000063 	0x63

0000032c <$LC33>:
 32c:	6e6e6163 	0x6e6e6163
 330:	6320746f 	0x6320746f
 334:	74616572 	jalx	18595c8 <main+0x185800c>
 338:	74732065 	jalx	1cc8194 <main+0x1cc6bd8>
 33c:	3a657461 	xori	a1,s3,0x7461
 340:	746f6e20 	jalx	1bdb880 <main+0x1bda2c4>
 344:	6f6e6520 	0x6f6e6520
 348:	20686775 	addi	t0,v1,26485
 34c:	6f6d656d 	0x6f6d656d
 350:	00007972 	0x7972
Disassembly of section .data.rel.local:

00000000 <progname>:
   0:	00000000 	nop
Disassembly of section .rodata.cst8:

00000000 <$LC19>:
   0:	00000000 	nop
   4:	407f7000 	0x407f7000
Disassembly of section .comment:

00000000 <.comment>:
   0:	43434700 	c0	0x1434700
   4:	4728203a 	c1	0x128203a
   8:	2029554e 	addi	t1,at,21838
   c:	2e342e33 	sltiu	s4,s1,11827
  10:	Address 0x0000000000000010 is out of bounds.

