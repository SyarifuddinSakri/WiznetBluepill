#include "main.h"
#include "stm32f1xx_hal.h"
#include "wizchip_conf.h"

extern SPI_HandleTypeDef hspi1;
wiz_NetInfo gNetInfoDefault = {
		.mac = 	{0x00, 0x08, 0xdc, 0xab, 0xcd, 0xed},//mac address
		.ip = {192,168,100,132},
		.sn = {255,255,255,0},
		.gw = {192,168,100,1},
		.dhcp = NETINFO_STATIC
};
wiz_PhyConf gwizPhy={
		.by = PHY_CONFBY_SW,
		.duplex = PHY_DUPLEX_FULL,
		.mode = PHY_MODE_AUTONEGO,
		.speed = PHY_SPEED_100
};

uint8_t SPIReadWrite(uint8_t data){
    //Wait till FIFO has free slot
    while((hspi1.Instance->SR & SPI_FLAG_TXE)!=SPI_FLAG_TXE);
    *(__IO uint8_t*)&hspi1.Instance->DR = data;

    //Now wait until all data has arrive
    while((hspi1.Instance->SR & SPI_FLAG_RXNE)!=SPI_FLAG_RXNE);
    
    return (*(__IO uint8_t*)&hspi1.Instance->DR);
    
}
void wizchip_select(void){
	HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET);
}

void wizchip_deselect(void){
	HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET);
}
// Single byte read function
uint8_t wizchip_read(void) {
    uint8_t rbuf;
    rbuf = SPIReadWrite(0x00);
    return rbuf;
}

// Single byte write function
void wizchip_write(uint8_t b) {
    SPIReadWrite(b);
}

// Read burst function
void wizchip_read_burst(uint8_t* pBuf, uint16_t len) {
    for (uint16_t i = 0; i < len; i++) {
        *pBuf = SPIReadWrite(0x00);
        pBuf++;
    }
}

// Write burst function
void wizchip_write_burst(uint8_t* pBuf, uint16_t len) {
    for (uint16_t i = 0; i < len; i++) {
        SPIReadWrite(*pBuf);
        pBuf++;
    }
}
void resetW5500(){
	HAL_GPIO_WritePin(RST_GPIO_Port, RST_Pin, GPIO_PIN_RESET);
	HAL_Delay(100);
	HAL_GPIO_WritePin(RST_GPIO_Port, RST_Pin, GPIO_PIN_SET);
}
void W5500Init(){
    uint8_t memsize[2][8] = {{2,2,2,2,2,2,2,2},{2,2,2,2,2,2,2,2}};

	HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET);//Chip Select high by default
	resetW5500();

	reg_wizchip_cs_cbfunc(wizchip_select, wizchip_deselect);
	reg_wizchip_spi_cbfunc(wizchip_read, wizchip_write);
	reg_wizchip_spiburst_cbfunc(wizchip_read_burst, wizchip_write_burst);

    if(ctlwizchip(CW_INIT_WIZCHIP, (void*) memsize)==-1){
        while(1){
                HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
                HAL_Delay(300);
        }
    }
	// ctlwizchip((CW_RESET_PHY | CW_RESET_WIZCHIP),(void*)0xff);
//	ctlwizchip(CW_CLR_INTERRUPT,(void*)(IK_WOL | IK_PPPOE_TERMINATED | IK_DEST_UNREACH | IK_IP_CONFLICT | IK_SOCK_ALL));
	// ctlnetwork(CN_SET_NETINFO, &gNetInfoDefault);
    // wizchip_setnetinfo(&gNetInfoDefault);

}
