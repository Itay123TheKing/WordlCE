from math import ceil


def bit(num, i):
	return bool(num & (1 << i))

with open('answers.txt', 'rb') as inp,\
	 open('answers.bin', 'wb') as out:
	arr = []
	while True:
		data = inp.read(1)
		if not data:
			break
		if data == b'\n':
			continue
		data = ord(data) - ord('a')
		for i in range(5):
			arr.append(bit(data, 4 - i))
	for i in range(len(arr) // 8):
		sum = 0
		for j in range(8):
			sum += arr[i * 8 + j] << (7 - j)
		out.write(sum.to_bytes(1, 'little'))
	if len(arr) % 8:
		sum = 0
		for j in range(len(arr) % 8):
			sum += arr[i * 8 + j] << (7 - j)
		out.write(sum.to_bytes(1, 'little'))
