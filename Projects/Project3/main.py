import cProfile
import os
import time
import random

# Constants
MAX_BLOCKS, FAIL, EMPTY, SUCCESS, REJECT, EOF, DIR_START, SIZE_PTR = 32768, -1, 0, 1, 2, -1, 0, 4
# Directory for io files

size_blocks = [8, 1024, 1024, 1024, 2048]
dir2 = os.getcwd() + '/io/input_1024_200_9_0_9.txt'
dirs = [
    os.getcwd() + '/io/input_8_600_5_5_0.txt',
    os.getcwd() + '/io/input_1024_200_5_9_9.txt',
    os.getcwd() + '/io/input_1024_200_9_0_0.txt',
    os.getcwd() + '/io/input_1024_200_9_0_9.txt',
    os.getcwd() + '/io/input_2048_600_5_5_0.txt'
]
# File ID, which is global, and updated
fileID_CA, fileID_LA = 1, 1


def getID_CA():
    """Updates and returns id of new file for Contigous Allocation"""
    global fileID_CA
    fileID_CA = fileID_CA + 1
    return fileID_CA


def getID_LA():
    """Updates and returns id of new file for Linked Allocation"""
    global fileID_LA
    fileID_LA = fileID_LA + 1
    return fileID_LA


class File:
    """File Class"""

    def __init__(self, idx, len_block, len_byte):
        """
        Initialize File Class object
        :param idx:index for file
        :param len_block: length of block
        :param len_byte: length of byte
        Args:
            idx:
            len_block:
            len_byte:
        """
        self.len_byte = len_byte
        self.len_block = len_block
        self.idx = idx


null_file = File(-1, -1, -1)


class DirectoryTable:
    """Directory Table class"""

    def __init__(self):
        """Initialize dictionary"""
        self.table = {}

    def update_idx(self, file_id, n_index):
        """
        Updates file index
        Args:
            file_id: id of file
            n_index: new index
        """
        if self.existence(file_id):
            self.table[file_id].idx = n_index
            return SUCCESS
        else:
            return FAIL

    def getFile(self, file_id):
        """
        Gets current file
        :param file_id: id of file
        Args:
            file_id:
        """
        if self.existence(file_id):
            return self.table[file_id]
        else:
            return null_file

    def add_file(self, file_ID, file):
        """
        Adds file corresponding file_ID
        Args:
            file_ID: id of file
            file: file to be added
        """
        if self.existence(file_ID):
            return FAIL
        self.table[file_ID] = file
        return SUCCESS

    def existence(self, file_ID):
        """
        Shows file given ID exist or not
        Args:
            file_ID: id of file
        """
        if file_ID in self.table:
            return True
        else:
            return False

    def update_len_block(self, file_ID, n_len):
        """
        Updates length of block
        Args:
            file_ID: id of file
            n_len: new lentgh of block
        """
        if self.existence(file_ID):
            self.table[file_ID].len_block = n_len
            return SUCCESS
        else:
            return FAIL

    def update_len_byte(self, file_ID, n_len):
        """
        Updates length of byte
        Args:
            file_ID: id of file
            n_len: new length of byte
        """
        if self.existence(file_ID):
            self.table[file_ID].len_byte = n_len
            return SUCCESS
        else:
            return FAIL

    def remove_file(self, file_ID):
        """
        Removes file from directory
        Args:
            file_ID: id of file
        """
        if self.existence(file_ID):
            self.table.pop(file_ID)
            return SUCCESS
        else:
            return FAIL


class FAT:
    """File Allocation Table Class"""

    def __init__(self):
        """
        Initialize content and next .
        Args:

        """
        self.next = EOF

    def empty(self):
        """Makes next End of file"""
        self.next = EOF

    def update(self, n):
        """
        Updates next index
        :param n: next index
        Args:
            n:
        """
        self.next = n


class ContiguousAllocation:
    """Contiguous Allocation Class"""

    def __init__(self, blockSize):
        """Given from input file block_size, initialize the Contiguous Allocation object
         :param blockSize: size of block

        Args:
            blockSize:
        """
        self.directory = [EMPTY] * MAX_BLOCKS
        self.block_size = blockSize
        self.available_space = MAX_BLOCKS
        self.table = DirectoryTable()

    def shift(self, file_id, size):
        """It shifts file left to right. if destination space is available.
            :param file_id: id of file
            :param size: amount of shift
            Args:
                file_id:
                size:
            """
        file = self.table.getFile(file_id)
        if file == null_file:
            return FAIL
        cnt = file.len_block + file.idx -1
        while file.idx <= cnt:
            if self.directory[cnt + size] != EMPTY:
                return FAIL
            self.directory[cnt] = EMPTY
            self.directory[cnt + size] = file_id
            cnt-=1
        self.table.update_idx(file_id, file.idx + size)


    def move(self, file_id, n_index):
        """It moves file to new index from start index if destination space is
        available.
        :param file_id: id of file
        :param n_index: new index

        Args:
            file_id:
            n_index:
        """
        file = self.table.getFile(file_id)
        if file == null_file:
            return FAIL
        old_idx = file.idx
        for i in range(file.len_block):
            if self.directory[n_index + i] != EMPTY:
                return FAIL
            self.directory[n_index + i] = file_id
            self.directory[old_idx + i] = EMPTY
        stat = self.table.update_idx(file_id, n_index)
        if stat == FAIL:
            return FAIL
        return SUCCESS

    def conversion(self, le):
        """Finds number of block required from length of bytes
        :param le:
        length of bytes

        Args:
            le:
        """
        return int((self.block_size + le - 1) / self.block_size)

    def is_extendable(self, idx, size):
        """It looks for whether a file is extendable or not at a given start index.
        :param size: extend size
        :param idx: selected to start index

        Args:
            idx:
            size:
        """
        for i in range(idx, (idx + size)):
            if self.directory[i] != EMPTY:
                return False

    def fit_space(self, num_block):
        """Looks for available space for files having a given number of blocks. Applies first fit strategy.
        :param num_block: required number of blocks

        Args:
            num_block:
        """
        cnt = 0
        while cnt < MAX_BLOCKS:
            sgn = False
            j = cnt + 1
            if self.directory[cnt] != EMPTY:
                cnt = j
                continue
            # Loop checks whether directory have enough empty space
            while j < MAX_BLOCKS and self.directory[j] == EMPTY:
                if (j - cnt) == num_block:
                    sgn = True
                    break
                j += 1
            if sgn:
                return cnt
            cnt = j
        return FAIL

    def compaction(self, s_idx):
        """In order to do compaction/defragmentation; All files in directory shifted from high to low indexes.
        This results in empty spaces being gathered at the end of the directory. With the help of compaction, now new
        files can be added. Because we have limited space, I implemented this function as shifting blocks 1-1. Start
        index helps to do compaction for extension because sometimes I move a file which is gonna extend to end.
        This causes empty space and I solve this by doing again compaction.
        :param s_idx: start index

        Args:
            s_idx:
        """
        tmp = s_idx  # tmp stores index which we want to do next insertion
        inc = 1
        for i in range(s_idx, MAX_BLOCKS, inc):
            inc = 1
            if self.directory[i] == EMPTY:
                continue
            if tmp == i:
                tmp += 1
                continue
            stat = self.move(self.directory[i], tmp)
            if stat == FAIL:
                return FAIL
            inc = self.table.getFile(self.directory[i]).len_block
            tmp += inc

        return SUCCESS

    def fill(self, idx, le):
        """It inserts file in directory with putting positive random integer in
        directory entry.
        :param idx: start index to fill
        :param le: length of block of file.

        Args:
            idx:
            le:
        """
        for i in range(idx, (idx + le)):
            if self.directory[i] != EMPTY:
                # print("Fill is impossible for id: {:d}, index: {:d}, length: {:d} ".format(file_ID, idx, len))
                return FAIL
            self.directory[i] = random.randint(1, MAX_BLOCKS)
        return SUCCESS

    def empty(self, idx, le):
        """It makes files that occupied by directory files empty.
        :param idx: start index
        :param le: length of block

        Args:
            idx:
            le:
        """
        for i in range(idx, (idx + le)):
            if self.directory[i] == EMPTY:
                # print("Empty is impossible due to having empty slot for id:%d, index:%d, length:%d ", file_ID, idx,
                # len)
                return FAIL
            self.directory[i] = EMPTY
        return SUCCESS

    # This function creates a new file.
    def create_file(self, file_ID, length):
        """This function allocates a space for the file of size file length
        bytes on the disk, updates the DT and the FAT. The file blocks may be
        filled with random number greater than zero. A suitable hole need to be
        found in contiguous allocation case. If no whole is found, you need to
        do compaction of the directory contents. If there is not
        enough space to store the file, the operation will be rejected with a
        warning message.
        :param file_ID: id of file
        :param length: length of
        block

        Args:
            file_ID:
            length:
        """
        if self.table.existence(file_ID):
            # print("New file can't created since file exists in call create_file for CA")
            return FAIL
        num_block = self.conversion(length)
        if num_block > self.available_space:
            # print("New file creation rejected since there is no sufficient memory in call create_file for CA")
            return REJECT
        idx = self.fit_space(num_block)
        if idx == FAIL:
            sta = self.compaction(DIR_START)
            if sta == FAIL:
                return FAIL
            idx = MAX_BLOCKS - self.available_space
        file_add = File(idx, num_block, length)
        stat = self.table.add_file(file_ID, file_add)
        if stat == FAIL:
            return FAIL
        stat = self.fill(idx, num_block)
        if stat == FAIL:
            return FAIL
        self.available_space = self.available_space - num_block
        return SUCCESS

    def access(self, file_ID, offset):
        """Returns the location of the byte having the given offset in the
        directory, where byte offset is the offset of that byte from the
        beginning of the file
        :param file_ID: id of file
        :param offset: byte offset

        Args:
            file_ID:
            offset:
        """
        if self.table.existence(file_ID):
            # print("File can't accessed since file doesn't exists in call access for CA")
            return FAIL
        file = self.table.getFile(file_ID)
        if file == null_file:
            return FAIL
        if file.len_byte < offset:
            # print("File can't accessed since offset exceeds file size in call access for CA")
            return FAIL
        return file.idx +  self.conversion(offset) - 1

    def extend(self, file_ID, exten_size):
        """Extends the given file by the given amount, where extension is the
        number of blocks not bytes. For simplicity, the extension will always
        add block after the last block of the file. If there is no sufficient
        space to extend the file, the operation will be rejected with a warning
        message. In contiguous allocation, if there is no contiguous space, you
        need to do compaction and may reallocate the file blocks. Remember that
        you have a buffer that can accommodate only single block.
        :param file_ID: id of file
        :param exten_size: amount of extension

        Args:
            file_ID:
            exten_size:
        """
        if self.table.existence(file_ID):
            # ("File can't extended since file doesn't exists in call extend for CA")
            return FAIL
        if self.available_space < exten_size:
            # print("File can't extended and rejected since out of memory in call extend for CA")
            return REJECT
        file = self.table.getFile(file_ID)
        if file == null_file:
            return FAIL
        if self.is_extendable(file.idx + file.len_block, exten_size):
            stat = self.fill(file.idx + file.len_block, exten_size)
            if stat == FAIL:
                return FAIL
        else:
            stat = self.compaction(DIR_START)
            if stat == FAIL:
                return FAIL
            file = self.table.getFile(file_ID)
            final_idx = file.len_block + file.idx -1
            idx = MAX_BLOCKS - self.available_space -1
            control = 1
            i = idx
            while final_idx < i:
                control = 1
                if self.directory[i] == EMPTY:
                    return FAIL
                stat = self.shift(self.directory[i], exten_size)
                if stat == FAIL:
                    return FAIL
                control = self.table.getFile(file_ID).len_block
                i -= control
                stat = self.fill(file.len_block + file.idx, exten_size)
                if stat == FAIL:
                    return FAIL
            self.table.update_len_block(file_ID, exten_size + file.len_block)
            self.table.update_len_byte(file_ID, exten_size * self.block_size + file.len_byte)
            self.available_space = self.available_space - exten_size
            return SUCCESS

    def shrink(self, file_ID, shrink_size):
        """Shrinks the file by the given number of blocks. The shrinking
        deallocates the last blocks of the file. Note that deallocation means
        just that these blocks are no more referred by that file and you can use
        them to store new data, and there is no need to move them or the files
        adjacent to them at the moment. You can indicate that block is freed by
        storing zero in it, knowing that you store random positive values in the
        filled blocks.
        :param file_ID: id of file
        :param shrink_size: amount of
        shrink

        Args:
            file_ID:
            shrink_size:
        """
        if self.table.existence(file_ID):
            # print("File can't shrinked since file doesn't exists in call shrink for CA")
            return FAIL
        if shrink_size == 0:
            # print("Size of shrink can't be 0 in call shrink for CA")
            return FAIL
        file = self.table.getFile(file_ID)
        if file == null_file:
            return FAIL
        if file.len_block < shrink_size:
            # print("Size of shrink bigger than file's length of block in call shrink for CA")
            return FAIL
        left = file.len_block - shrink_size
        stat = self.empty(file.idx + left, shrink_size)
        if stat == FAIL:
            return FAIL
        if left == 0:
            stat = self.table.remove_file(file_ID)
            if stat == FAIL:
                return FAIL
        else:
            self.table.update_len_block(file_ID, left)
            self.table.update_len_byte(file_ID, file.len_byte - self.block_size * shrink_size)
        self.available_space = self.available_space + shrink_size
        return SUCCESS


class LinkedAllocation:
    """Linked Allocation Class"""

    def __init__(self, blockSize):
        """Given from input file block_size, initialize the Linked Allocation object.
        :param blockSize: size of block

        Args:
            blockSize:
        """
        self.directory_content = [EMPTY] * MAX_BLOCKS
        self.directory = [FAT()] * MAX_BLOCKS
        # I excluded the size of FAT as stating size of PTR from block size since we have limited memory.
        self.block_size = (-1*SIZE_PTR) + blockSize  
        self.available_space = MAX_BLOCKS
        self.table = DirectoryTable()

    def create_file(self, file_ID, len_file):
        """
            This function allocates a space for the file

        of size file length bytes on the disk, updates the DT and the FAT.
        The file blocks may be filled with random number greater than zero. A
        suitable hole need to be found in contiguous allocation case. If no
        whole is found, you need to do compaction/defragmentation of the
        directory contents. If there is not enough space to store the file, the
        operation will be rejected with a warning message. :param file_ID: id of
        file :param len_file: length of block:

        Args:
            file_ID:
            len_file:
        """
        if self.table.existence(file_ID):
            # print("New file can't created since file exists in call create_file for LA")
            return FAIL
        num_block = self.conversion(len_file)
        if num_block > self.available_space:
            # print("New file creation rejected since there is no sufficient memory in call create_file for LA")
            return REJECT
        idxs = self.fit_space(num_block)
        if len(idxs) != num_block:
            # print("Not enough free slots in call create_file for LA")
            return FAIL
        for count, idx in enumerate(idxs, 0):
            if self.directory_content[idx] != EMPTY:
                # print("Want to access empty space but failed in call create_file for LA")
                return FAIL
            self.directory_content[idx] = random.randint(1, MAX_BLOCKS)
            # self.directory[idx].set(random.randint(1, MAX_BLOCKS))
            if count + 1 < num_block:
                self.directory[idx].update(idxs[count + 1])
        stat = self.table.add_file(file_ID, File(idxs[0], num_block, len_file))
        if stat == FAIL:
            return FAIL
        self.available_space = self.available_space - num_block
        return SUCCESS

    def conversion(self, le):
        """Finds number of block required from length of bytes :param len:
        length of bytes

        Args:
            len:
        """
        return int((-1 + self.block_size + le) / self.block_size)

    def fit_space(self, n_block):
        """Looks for available block for file having given number of block.
        :param n_block: required number of blocks

        Args:
            n_block:
        """
        s = []
        for i in range(MAX_BLOCKS):
            if self.directory_content[i] == EMPTY:
                s.append(i)
            if len(s) == n_block:
                return s
        return s

    def access(self, file_ID, offset):
        """Returns the location of the byte having the given offset in the
        directory, where byte offset is the offset of that byte from the
        beginning of the file :param file_ID: id of file :param offset: byte
        offset

        Args:
            file_ID:
            offset:
        """
        if self.table.existence(file_ID):
            # print("File can't accessed since file does not exists in call access for LA")
            return FAIL
        file = self.table.getFile(file_ID)
        if file == null_file:
            return FAIL
        if file.len_byte < offset:
            # print("Byte offset exceeds in call access for LA")
            return FAIL
        idx = self.table.getFile(file_ID).idx
        while self.block_size < offset:
            offset = offset - self.block_size
            idx = self.directory[idx].next
        return idx

    def extend(self, file_ID, exten_size):
        """Extends the given file by the given amount, where extension is the
        number of blocks not bytes. For simplicity, the extension will always
        add block after the last block of the file. If there is no sufficient
        space to extend the file, the operation will be rejected with a warning
        message. In contiguous allocation, if there is no contiguous space, you
        need to do compaction and may reallocate the file blocks. Remember that
        you have a buffer that can accommodate only single block.
        :param file_ID: id of file
        :param exten_size: amount of extension

        Args:
            file_ID:
            exten_size:
        """
        if self.table.existence(file_ID):
            # print("File can't extended since file doesn't exists in call extend for LA")
            return FAIL
        if self.available_space < exten_size:
            # print("File can't extended and rejected since out of memory in call extend for LA")
            return REJECT
        file = self.table.getFile(file_ID)
        if file == null_file:
            return FAIL
        idxs = self.fit_space(exten_size)
        if len(idxs) != exten_size:
            # print("There is not enough free slots in call extend for LA")
            return FAIL
        index = file.idx
        while self.directory[index].next != EOF:
            index = self.directory[index].next
        self.directory[index].next = idxs[0]
        for count, idx in enumerate(idxs, 0):
            if self.directory_content[idx] != EMPTY:
                # print("There is not empty space in directory in call extend for LA")
                return FAIL
            self.directory_content[idx] = random.randint(1, MAX_BLOCKS)
            if (count + 1) < len(idxs):
                self.directory[idx].update(idxs[count + 1])
        self.table.update_len_block(file_ID, file.len_block + exten_size)
        self.table.update_len_byte(file_ID, file.len_byte + self.block_size * exten_size)
        self.available_space = self.available_space - exten_size
        return SUCCESS

    def shrink(self, file_ID, shrink_size):
        """Shrinks the file by the given number of blocks. The shrinking
        deallocates the last blocks of the file. Note that deallocation means
        just that these blocks are no more referred by that file and you can use
        them to store new data, and there is no need to move them or the files
        adjacent to them at the moment. You can indicate that block is freed by
        storing zero in it, knowing that you store random positive values in the
        filled blocks. :param file_ID: id of file :param shrink_size: amount of
        shrink

        Args:
            file_ID:
            shrink_size:
        """
        if self.table.existence(file_ID):
            # print("File can't shrinked since file doesn't exists in call shrink for LA")
            return FAIL
        if shrink_size == 0:
            # print("Size of shrink can't be 0 in call shrink for LA")
            return REJECT
        file = self.table.getFile(file_ID)
        if file == null_file:
            return FAIL
        if file.len_block < shrink_size:
            # print("Size of shrink bigger than file's length of block in call shrink for LA")
            return FAIL
        left = file.len_block - shrink_size
        idx = file.idx
        sign = False
        if left <= 1:
            self.table.update_len_block(file_ID, 1)
            self.table.update_len_byte(file_ID, self.block_size)
            new = self.directory[idx].next
            self.directory[idx].update(EOF)
            idx = new
            sign = True
        else:
            self.table.update_len_block(file_ID, file.len_block - shrink_size)
            self.table.update_len_byte(file_ID, file.len_byte - self.block_size * shrink_size)
            for i in range(left - 1):
                idx = self.directory[idx].next
            new = self.directory[idx].next
            self.directory[idx].update(EOF)
            idx = new
        cnt = 0
        while idx != EOF:
            cnt = cnt + 1
            new = self.directory[idx].next
            self.directory_content[idx] = EMPTY
            self.directory[idx].empty()
            idx = new
        self.available_space = self.available_space + cnt
        return SUCCESS


def run(CA, LA, dir_idx, idx):
    """
    Args:
        CA: Contigous Allocation Class object
        LA: Linked Allocation Class object
        dir_idx: index of io file
        idx: repeat number
    """
    global fileID_CA
    fileID_CA = 1
    global fileID_LA
    fileID_LA = 1
    num_create_CA, num_access_CA, num_extend_CA, num_shrink_CA = 0, 0, 0, 0
    num_create_LA, num_access_LA, num_extend_LA, num_shrink_LA = 0, 0, 0, 0
    num_create_reject_CA, num_create_reject_LA, num_extend_reject_CA, num_extend_reject_LA = 0, 0, 0, 0
    create_time_CA, create_time_LA, access_time_CA, access_time_LA, extend_time_CA, extend_time_LA, shrink_time_CA, \
    shrink_time_LA = 0, 0, 0, 0, 0, 0, 0, 0
    i_time_all = time.time_ns()
    with open(dir_idx) as fp:
        for cnt, line in enumerate(fp):
            cmd = line.split(':')
            if cmd[0] == 'c':
                i_c_time_CA = time.time_ns()
                stat_CA = CA.create_file(getID_CA(), int(cmd[1]))
                f_c_time_CA = time.time_ns()
                i_c_time_LA = time.time_ns()
                stat_LA = LA.create_file(getID_LA(), int(cmd[1]))
                f_c_time_LA = time.time_ns()
                if stat_CA == REJECT:
                    num_create_reject_CA = num_create_reject_CA + 1
                if stat_LA == REJECT:
                    num_create_reject_LA = num_create_reject_LA + 1
                create_time_CA = create_time_CA + f_c_time_CA - i_c_time_CA
                create_time_LA = create_time_LA + f_c_time_LA - i_c_time_LA
                num_create_CA = num_create_CA + 1
                num_create_LA = num_create_LA + 1
                continue
            if cmd[0] == 'a':
                i_a_time_CA = time.time_ns()
                stat_CA = CA.access(int(cmd[1]), int(cmd[2]))
                f_a_time_CA = time.time_ns()
                i_a_time_LA = time.time_ns()
                stat_LA = LA.access(int(cmd[1]), int(cmd[2]))
                f_a_time_LA = time.time_ns()
                access_time_CA = access_time_CA + f_a_time_CA - i_a_time_CA
                access_time_LA = access_time_LA + f_a_time_LA - i_a_time_LA
                num_access_CA = num_access_CA + 1
                num_access_LA = num_access_LA + 1
                continue
            if cmd[0] == 'e':
                i_e_time_CA = time.time_ns()
                stat_CA = CA.extend(int(cmd[1]), int(cmd[2]))
                f_e_time_CA = time.time_ns()
                i_e_time_LA = time.time_ns()
                stat_LA = LA.extend(int(cmd[1]), int(cmd[2]))
                f_e_time_LA = time.time_ns()
                if stat_CA == REJECT:
                    num_extend_reject_CA = num_extend_reject_CA + 1
                if stat_LA == REJECT:
                    num_extend_reject_LA = num_extend_reject_LA + 1
                extend_time_CA = extend_time_CA + f_e_time_CA - i_e_time_CA
                extend_time_LA = extend_time_LA + f_e_time_LA - i_e_time_LA
                num_extend_CA = num_extend_CA + 1
                num_extend_LA = num_extend_LA + 1
                continue

            if cmd[0] == 'sh':
                i_sh_time_CA = time.time_ns()
                stat_CA = CA.shrink(int(cmd[1]), int(cmd[2]))
                f_sh_time_CA = time.time_ns()
                i_sh_time_LA = time.time_ns()
                stat_LA = LA.shrink(int(cmd[1]), int(cmd[2]))
                f_sh_time_LA = time.time_ns()
                shrink_time_CA = shrink_time_CA + f_sh_time_CA - i_sh_time_CA
                shrink_time_LA = shrink_time_LA + f_sh_time_LA - i_sh_time_LA
                num_shrink_CA = num_shrink_CA + 1
                num_shrink_LA = num_shrink_LA + 1
                continue
        if num_create_CA != 0:
            create_time_CA = create_time_CA / num_create_CA
        if num_create_LA != 0:
            create_time_LA = create_time_LA / num_create_LA
        if num_access_CA != 0:
            access_time_CA = access_time_CA / num_access_CA
        if num_access_LA != 0:
            access_time_LA = access_time_LA / num_access_LA
        if num_extend_CA != 0:
            extend_time_CA = extend_time_CA / num_extend_CA
        if num_extend_LA != 0:
            extend_time_LA = extend_time_LA / num_extend_LA
        if num_shrink_CA != 0:
            shrink_time_CA = shrink_time_CA / num_shrink_CA
        if num_shrink_LA != 0:
            shrink_time_LA = shrink_time_LA / num_shrink_LA
        f_time_all = time.time_ns()
        time_runned = f_time_all - i_time_all
    return create_time_CA, create_time_LA, access_time_CA, access_time_LA, extend_time_CA, extend_time_LA, \
           shrink_time_CA, shrink_time_LA, time_runned, num_create_reject_CA, num_create_reject_LA, num_extend_reject_CA, \
           num_extend_reject_LA


def print_log(create_time_CA, create_time_LA, access_time_CA, access_time_LA, extend_time_CA, extend_time_LA,
              shrink_time_CA, shrink_time_LA, time_runned, num_create_reject_CA, num_create_reject_LA,
              num_extend_reject_CA, num_extend_reject_LA, idx):
    """
    Args:
        create_time_CA: time of create for Contigous Allocation
        create_time_LA: time of create for Linked Allocation
        access_time_CA: time of access for Contigous Allocation
        access_time_LA: time of access for Linked Allocation
        extend_time_CA: time of extend for Contigous Allocation
        extend_time_LA: time of extend for Linked Allocation
        shrink_time_CA: time of shrink for Contigous Allocation
        shrink_time_LA: time of shrink for Linked Allocation
        time_runned: total time
        num_create_reject_CA: number of rejection rate of create for Contigous
            Allocation
        num_create_reject_LA: number of rejection rate of create for Linked
            Allocation
        num_extend_reject_CA: number of rejection rate of extend for Contigous
            Allocation
        num_extend_reject_LA: number of rejection rate of extend for Linked
            Allocation
        idx: index of io file
    """
    print("-------------------SAMPLE-{:d}--------------------------------------------------\n".format(idx))
    print("Average rejection number for creation CA: {:d}\n".format(int(num_create_reject_CA)))
    print("Average rejection number for creation LA: {:d}\n".format(int(num_create_reject_LA)))
    print("Average rejection number for extend CA: {:d}\n".format(int(num_extend_reject_CA)))
    print("Average rejection number for extend LA: {:d}\n".format(int(num_extend_reject_LA)))
    print("Average time for creation CA: {:f} nanosecond\n".format(create_time_CA))
    print("Average time for creation LA: {:f} nanosecond\n".format(create_time_LA))
    print("Average time for access CA: {:f} nanosecond\n".format(access_time_CA))
    print("Average time for access LA: {:f} nanosecond\n".format(access_time_LA))
    print("Average time for extend CA: {:f} nanosecond\n".format(extend_time_CA))
    print("Average time for extend LA: {:f} nanosecond\n".format(extend_time_LA))
    print("Average time for shrink CA: {:f} nanosecond\n".format(shrink_time_CA))
    print("Average time for shrink LA: {:f} nanosecond\n".format(shrink_time_LA))


def main():
    os.chdir(os.getcwd())
    # filepath = sys.argv[1]
    pr = cProfile.Profile()
    pr.enable()
    for cnt, dir_idx in enumerate(dirs, 0):
        tot_create_time_CA, tot_access_time_CA, tot_extend_time_CA, tot_shrink_time_CA = 0, 0, 0, 0
        tot_create_time_LA, tot_access_time_LA, tot_extend_time_LA, tot_shrink_time_LA = 0, 0, 0, 0
        tot_num_create_reject_CA, tot_num_create_reject_LA, tot_num_extend_reject_CA, tot_num_extend_reject_LA = 0, 0, 0, 0
        block = size_blocks[cnt]
        CA = ContiguousAllocation(block)
        LA = LinkedAllocation(block)
        for i in range(5):
            create_time_CA, create_time_LA, access_time_CA, access_time_LA, extend_time_CA, extend_time_LA, \
            shrink_time_CA, shrink_time_LA, time_runned, num_create_reject_CA, num_create_reject_LA, num_extend_reject_CA, \
            num_extend_reject_LA = run(CA, LA, dir_idx, i)
            tot_create_time_CA += create_time_CA
            tot_create_time_LA += create_time_LA
            tot_access_time_CA += access_time_CA
            tot_access_time_LA += access_time_LA
            tot_extend_time_CA += extend_time_CA
            tot_extend_time_LA += extend_time_LA
            tot_shrink_time_CA += shrink_time_CA
            tot_shrink_time_LA += shrink_time_LA
            tot_num_create_reject_CA += num_create_reject_CA
            tot_num_create_reject_LA += num_create_reject_LA
            tot_num_extend_reject_CA += num_extend_reject_CA
            tot_num_extend_reject_LA += num_extend_reject_LA
        tot_create_time_CA /= 5
        tot_create_time_LA /= 5
        tot_access_time_CA /= 5
        tot_access_time_LA /= 5
        tot_extend_time_CA /= 5
        tot_extend_time_LA /= 5
        tot_shrink_time_CA /= 5
        tot_shrink_time_LA /= 5
        tot_num_create_reject_CA /= 5
        tot_num_create_reject_LA /= 5
        tot_num_extend_reject_CA /= 5
        tot_num_extend_reject_LA /= 5

        print_log(tot_create_time_CA, tot_create_time_LA, tot_access_time_CA, tot_access_time_LA, tot_extend_time_CA,
                  tot_extend_time_LA, tot_shrink_time_CA, tot_shrink_time_LA, time_runned, tot_num_create_reject_CA,
                  tot_num_create_reject_LA,
                  tot_num_extend_reject_CA, tot_num_extend_reject_LA, (cnt + 1))

    pr.disable()
    pr.print_stats()


main()
