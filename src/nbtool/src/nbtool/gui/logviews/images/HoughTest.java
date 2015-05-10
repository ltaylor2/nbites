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

public class HoughTest extends ViewParent implements CppFuncListener{
	private VisionFieldOuterClass.Lines houghLines;
	private VisionFieldOuterClass.BoundaryLines fieldLines;
    private BufferedImage yImg;
    private BufferedImage edgeImg;
    private BufferedImage fieldEdgeImg;
    private BufferedImage gImg;
    private BufferedImage fImg;


	public void paintComponent(Graphics g) {
        if (yImg != null) {
		    g.drawImage(yImg, 0, 0, null);
        }
		if (edgeImg != null) {
			g.drawImage(edgeImg, 0, 280, null);
		}
		if (fieldEdgeImg != null) {
			g.drawImage(fieldEdgeImg, 0, 560, null);
		}
		if (gImg != null) {
			g.drawImage(gImg, 400, 0, null);
		}
		if (fImg != null) {
			g.drawImage(fImg, 400, 280, null);
		}

        List<VisionFieldOuterClass.Lines.Line> lines = houghLines.getLineList();
        float r;
        float t;
        double x0, y0;
        int x1, y1, x2, y2;
        double end0, end1;

        for (int i = 0; i < lines.size(); i++) {
        	if (i % 2 == 0)
        		g.setColor(java.awt.Color.red);
        	else
        		g.setColor(java.awt.Color.blue);

            r = lines.get(i).getRadius();
            t = lines.get(i).getAngle();
            end0 = lines.get(i).getEnd0();
            end1 = lines.get(i).getEnd1();
            
            x0 = r * Math.cos(t) + yImg.getWidth() / 2;
            y0 = -r * Math.sin(t) + yImg.getHeight() / 2;
            x1 = (int)Math.round(x0 + end0 * Math.sin(t));
            y1 = (int)Math.round(y0 + end0 * Math.cos(t));
            x2 = (int)Math.round(x0 + end1 * Math.sin(t));
            y2 = (int)Math.round(y0 + end1 * Math.cos(t));
            g.drawLine(x1, y1, x2, y2);
      	} 

      	List<VisionFieldOuterClass.BoundaryLines.Line> bLines = fieldLines.getLineList();
      	g.setColor(java.awt.Color.green);
      	for (int i = 0; i < bLines.size(); i++) {
      		// NOTE image is being drawn 400 to the right on the view
      		// for independent study display purposes
            g.drawLine((int)bLines.get(i).getX1() + 400, (int)bLines.get(i).getY1(),
            		   (int)bLines.get(i).getX2() + 400, (int)bLines.get(i).getY2());
      	}
    }
	
	@Override
	public void setLog(Log newlog) {
		log = newlog;

		int fi = CppIO.current.indexOfFunc("HoughLine");
		if (fi < 0) return;
		
		CppFuncCall fc = new CppFuncCall();
		
		fc.index = fi;
		fc.name = "HoughLine";
		fc.args = new ArrayList<Log>(Arrays.asList(log));
		fc.listener = this;
		
		CppIO.current.tryAddCall(fc);
	}
	
	public HoughTest() {
		super();
	}

	@Override
	public void returned(int ret, Log... out) { 
		this.yImg = U.biFromLog(out[0]);
		this.edgeImg = U.biFromLog(out[2]);
		this.fieldEdgeImg = U.biFromLog(out[3]);
		this.gImg = U.biFromLog(out[4]);
		this.fImg = U.biFromLog(out[5]);
        System.out.print("image loaded");
        try {
		    this.houghLines = VisionFieldOuterClass.Lines.parseFrom(out[1].bytes);
        } catch (InvalidProtocolBufferException e) {
            System.out.print("proto was not valid\n");
        }
        try {
        	this.fieldLines = VisionFieldOuterClass.BoundaryLines.parseFrom(out[6].bytes);
        } catch (InvalidProtocolBufferException e) {
        	System.out.print("proto was not valid\n");
        }

		repaint();
	}
}
