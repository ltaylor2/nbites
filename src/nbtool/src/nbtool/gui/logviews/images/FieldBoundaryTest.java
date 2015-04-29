package nbtool.gui.logviews.images;

import java.lang.Math;
import java.util.List;

import java.awt.Graphics;
import java.awt.image.BufferedImage;
import java.util.ArrayList;
import java.util.Arrays;

import nbtool.data.Log;
import nbtool.io.CppIO;
import nbtool.io.CppIO.CppFuncCall;
import nbtool.io.CppIO.CppFuncListener;
import nbtool.util.U;
import nbtool.gui.logviews.misc.ViewParent;

import messages.VisionFieldOuterClass;
import com.google.protobuf.InvalidProtocolBufferException;

public class FieldBoundaryTest extends ViewParent implements CppFuncListener{
    private BufferedImage gImg;
    private BufferedImage fImg;
	private VisionFieldOuterClass.BoundaryLines boundaryLines;

	public void paintComponent(Graphics g) {
        if(gImg != null) {
		    g.drawImage(gImg, 0, 0, null);
        }
		if(fImg != null) {
			g.drawImage(fImg, 0, 400, null);
		}

        List<VisionFieldOuterClass.BoundaryLines.Line> lines = boundaryLines.getLineList();
        float r;
        float t;
        double x0, y0;
        int x1, y1, x2, y2;
        double end0, end1;
        g.setColor(java.awt.Color.red);

        System.out.println("");
        for(int i = 0; i < lines.size(); i++) {
            g.drawLine((int)lines.get(i).getX1(), (int)lines.get(i).getY1(),
            		   (int)lines.get(i).getX2(), (int)lines.get(i).getY2());
      } 
    }
	
	@Override
	public void setLog(Log newlog) {
		log = newlog;

		int fi = CppIO.current.indexOfFunc("FieldBoundary");
		if (fi < 0) return;
		
		CppFuncCall fc = new CppFuncCall();
		
		fc.index = fi;
		fc.name = "FieldBoundary";
		fc.args = new ArrayList<Log>(Arrays.asList(log));
		fc.listener = this;
		
		CppIO.current.tryAddCall(fc);
	}
	
	public FieldBoundaryTest() {
		super();
	}

	@Override
	public void returned(int ret, Log... out) { 
		this.gImg = U.biFromLog(out[0]);
		this.fImg = U.biFromLog(out[1]);
        System.out.print("images loaded");

        try {
		    this.boundaryLines = VisionFieldOuterClass.BoundaryLines.parseFrom(out[2].bytes);
        } catch (InvalidProtocolBufferException e) {
            System.out.print("proto was not valid\n");
        }
		repaint();
	}
}
