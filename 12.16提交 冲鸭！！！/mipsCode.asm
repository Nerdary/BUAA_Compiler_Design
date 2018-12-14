.data
.text

addi $gp, $gp, -4
li $t1, 5
sw $t1, 0($gp)
addi $gp, $gp, -4
li $t1, 1
sw $t1, 0($gp)
addi $gp, $gp, -4
li $t1, 99
sw $t1, 0($gp)
addi $gp, $gp, -4
li $t1, 43
sw $t1, 0($gp)
addi $gp, $gp, -4
li $t1, 4
li $t2, 10
mul $t3, $t1, $t2
sub $gp, $gp, $t3
addi $gp, $gp, -4
li $t1, 4
li $t2, 12
mul $t3, $t1, $t2
sub $gp, $gp, $t3
