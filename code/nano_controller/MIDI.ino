void sendMIDImessage(byte statusByte, byte data1, byte data2){ //three byte message
  Serial1.write(statusByte);
  Serial1.write(data1);
  Serial1.write(data2);
}

void sendMIDImessage(byte statusByte, byte data1){ //two byte message (program change)
  Serial1.write(statusByte);
  Serial1.write(data1);
}
