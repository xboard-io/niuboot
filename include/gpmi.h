//unsigned int gpmi_dm9000_read_reg(unsigned int regno);
//void gpmi_dm9000_write_reg(unsigned int regno, unsigned int regval);
void gpmi_dm9000_write_reg_index(unsigned int index);
unsigned int gpmi_k9f1208_read_id(void);
unsigned int gpmi_dm9000_write_data_bulk(unsigned char *buf, int count);
unsigned int gpmi_dm9000_read_data_bulk(unsigned char *buf, int count);
unsigned int gpmi_k9f1208_read_page(int block, int page, char*buf);



