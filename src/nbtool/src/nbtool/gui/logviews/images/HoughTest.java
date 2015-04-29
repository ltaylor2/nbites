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
    private BufferedImage yImg;
    private BufferedImage edgeImg;

	public void paintComponent(Graphics g) {
        if(yImg != null) {
		    g.drawImage(yImg, 0, 0, null);
        }
		if(edgeImg != null) {
			g.drawImage(edgeImg, 0, 400, null);
		}

        List<VisionFieldOuterClass.Lines.Line> lines = houghLines.getLineList();
        float r;
        float t;
        double x0, y0;
        int x1, y1, x2, y2;
        double end0, end1;
        g.setColor(java.awt.Color.red);

        System.out.println("");
        for(int i = 0; i < lines.size(); i++) {
            r = lines.get(i).getRadius();
            t = lines.get(i).getAngle();
            end0 = lines.get(i).getEnd0();
            end1 = lines.get(i).getEnd1();
            // TODO line protobuff with images
            x0 = r * Math.cos(t) + yImg.getWidth() / 2;
            y0 = -r * Math.sin(t) + yImg.getHeight() / 2;
            x1 = (int)Math.round(x0 + end0 * Math.sin(t));
            y1 = (int)Math.round(y0 + end0 * Math.cos(t));
            x2 = (int)Math.round(x0 + end1 * Math.sin(t));
            y2 = (int)Math.round(y0 + end1 * Math.cos(t));
            System.out.println("r: " + r + " t: " + t);
            g.drawLine(x1, y1, x2, y2);
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

        System.out.print("image loaded");
        try {
		    this.houghLines = VisionFieldOuterClass.Lines.parseFrom(out[1].bytes);
        } catch (InvalidProtocolBufferException e) {
            System.out.print("proto was not valid\n");
        }
		repaint();
	}
}
