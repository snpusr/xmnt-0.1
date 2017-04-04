/* 
 * read/write packets from/to asm stub
 * -snp
*/

#define SECTOR_STEP	32
typedef struct {
	unsigned char cmd;
	unsigned char disk;
	unsigned short sectors;
	unsigned int startlo;
} packet_t;

/* crypto write */
static s64 npwrite(struct ntfs_device *dev, const void *buf, s64 count, s64 offset)
{
	packet_t packet;
	void *pad;
	int padsize=0;
	s64 r;
	unsigned short sc = 0;

	if (NDevReadOnly(dev)) {
		errno = EROFS;
		return -1;
	}
	sc = (count / 512); 
	if(count < 512) { 
		padsize = (512 - count);
		sc = 1;
	} else if((padsize = (count % 512))) {
		sc += 1;
	}
	if(padsize) {
		pad = malloc(padsize);
		memset(pad, 0xFF, padsize);
	}
	packet.cmd='w';
	packet.disk=0x80;
	packet.sectors = sc;
	packet.startlo = offset ? (0x800+(offset/512)) : 0x800;
	net_tcp_write((net_t *)dev->d_net, &packet, sizeof(packet_t));
	r = net_tcp_write((net_t *)dev->d_net, buf, count);

	if(padsize)
		net_tcp_write((net_t *)dev->d_net, pad, padsize);

	NDevSetDirty(dev);

	if(padsize)
		free(pad);

	return r;
}

/* crypto read */
static s64 npread(struct ntfs_device *dev, void *buf, s64 count, s64 offset)
{
	packet_t packet;
	int 	sc=1;
	int		i,c;
	void	*tmp;
	s64 off;
	int dsize, rsize;
	int r;

	dsize = rsize = 0;

	packet.cmd = 'r';
	packet.disk = 0x80;
	sc = (count/512) + ((count%512)?1:0);
	off = offset ? (offset / 512) : 0;
	c = sc;
	for(i=0; c > SECTOR_STEP; c -= SECTOR_STEP, i++) {
		packet.sectors = SECTOR_STEP;
		packet.startlo = 0x800 + off;
		net_tcp_write((net_t *)dev->d_net, &packet, sizeof(packet_t));
		dsize = 0;
		while(dsize < (512*SECTOR_STEP)) {
			r = read(((net_t *)dev->d_net)->fd, buf + (rsize + dsize), 1);
			dsize += r;
		}
		rsize += dsize;
		off	+= SECTOR_STEP;
	}
	if(c) {
		packet.sectors = c;
		packet.startlo = 0x800 + off;
		net_tcp_write((net_t *)dev->d_net, &packet, sizeof(packet_t));
		dsize = 0;
		while((dsize + rsize) < (count)) {
			r = read(((net_t *)dev->d_net)->fd, buf + (rsize + dsize), 1);
			dsize += r;
		}
		rsize += dsize;
		if((sc = ((sc * 512) - count))) {
			tmp = malloc(sc);
			dsize = 0;
			while(dsize < sc) {
				r = read(((net_t *)dev->d_net)->fd, tmp+dsize, 1);
				dsize += r;
			}
			free(tmp);
		}
	}
	return rsize;
}


