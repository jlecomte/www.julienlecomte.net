package com.yahoo.platform.build.ant;

import org.apache.tools.ant.BuildException;
import org.apache.tools.ant.DirectoryScanner;
import org.apache.tools.ant.Task;
import org.apache.tools.ant.types.FileSet;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.MappedByteBuffer;
import java.nio.channels.FileChannel;
import java.util.Iterator;
import java.util.Properties;
import java.util.Vector;
import java.util.zip.CRC32;
import java.util.zip.CheckedInputStream;

/**
 * Use this task like this:
 * <p/>
 * <taskdef name="FileTransform"
 * classname="com.yahoo.platform.build.ant.FileTransform"
 * classpath="${build.dir}/tools/classes"/>
 * <p/>
 * <FileTransform todir="${build.dir}/intermediate/js">
 * <fileset dir="${build.dir}/intermediate/js">
 * <include name="*.js"/>
 * </fileset>
 * </FileTransform>
 * <p/>
 * This task will copy the files specified in the nested fileset, will rename them
 * (replace their base name by the value of their checksum and keep the same extension)
 * and will set an ant property which name is the base name of the original file, and
 * the value is the full name of the destination file.
 */
public class FileTransform extends Task {

    private String todir;
    private boolean changefilenames;
    private String propertiesfile;
    private Vector filesets = new Vector();

    public void setTodir(String todir) {
        this.todir = todir;
    }

    public void setChangefilenames(boolean changefilenames) {
        this.changefilenames = changefilenames;
    }

    public void setPropertiesfile(String propertiesfile) {
        this.propertiesfile = propertiesfile;
    }

    public void addFileset(FileSet fileset) {
        filesets.add(fileset);
    }

    protected void validate() {
        if (todir == null)
            throw new BuildException("attribute \"todir\" not set");
        File dest = new File(todir);
        if (!dest.exists() || !dest.isDirectory())
            throw new BuildException("the value specified by the attribute \"todir\" is not valid");
        if (filesets.size() < 1)
            throw new BuildException("fileset not set");
    }

    public void execute() {
        validate();
        Properties properties = new Properties();
        for (Iterator itFSets = filesets.iterator(); itFSets.hasNext();) {
            FileSet fs = (FileSet) itFSets.next();
            DirectoryScanner ds = fs.getDirectoryScanner(getProject());
            String[] includedFiles = ds.getIncludedFiles();
            for (int i = 0; i < includedFiles.length; i++) {
                String srcFilename = includedFiles[i].replace('\\', '/');
                srcFilename = srcFilename.substring(srcFilename.lastIndexOf("/") + 1);
                File srcFile = new File(ds.getBasedir(), includedFiles[i]);

                try {
                    String dstFilename;
                    if (changefilenames) {
                        // Calculate the CRC-32 checksum of this file
                        CheckedInputStream cis = new CheckedInputStream(new FileInputStream(srcFile), new CRC32());
                        byte[] tempBuf = new byte[128];
                        while (cis.read(tempBuf) >= 0) {
                        }
                        long checksum = cis.getChecksum().getValue();
                        // Get the file extension
                        int idx = srcFilename.lastIndexOf('.');
                        String extension = "";
                        if (idx != -1) {
                            extension = srcFilename.substring(idx);
                        }
                        // Compute the new filename
                        dstFilename = String.valueOf(checksum) + extension;
                    } else {
                        dstFilename = srcFilename;
                    }
                    //
                    // Copy the file
                    //
                    File dstDir = new File(todir);
                    File dstFile = new File(dstDir, dstFilename);
                    copy(srcFile, dstFile);
                    //
                    // Set the new property...
                    //
                    String propertyValue = dstFilename;
                    getProject().setNewProperty(srcFilename, propertyValue);
                    properties.setProperty(srcFilename, propertyValue);

                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }

        if (propertiesfile != null) {
            try {
                properties.store(new FileOutputStream(new File(propertiesfile)), null);
            } catch (IOException e) {
                e.printStackTrace();
                System.exit(1);
            }
        }
    }

    private static void copy(File source, File dest) throws IOException {
        FileChannel in = null, out = null;
        try {
            in = new FileInputStream(source).getChannel();
            out = new FileOutputStream(dest).getChannel();
            long size = in.size();
            MappedByteBuffer buf = in.map(FileChannel.MapMode.READ_ONLY, 0, size);
            out.write(buf);
        } finally {
            if (in != null) in.close();
            if (out != null) out.close();
        }
    }
}
