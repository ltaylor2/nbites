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
    private BufferedImage magImg;
    private BufferedImage edgeImg;

	public void paintComponent(Graphics g) {
        if(yImg != null)
		    g.drawImage(yImg, 0, 0, null);
		if(magImg != null)
			g.drawImage(magImg, 0, 400, null);
		if(edgeImg != null)
			g.drawImage(edgeImg,400, 400, null);

        List<VisionFieldOuterClass.Line> lines = houghLines.getLinesList();
        float r;
        float t;
        double x1;
        double y1;
        g.setColor(java.awt.Color.red);

        System.out.println("");
        for(int i = 0; i < lines.size(); i++) {
            r = lines.get(i).getRadius();
            t = lines.get(i).getAngle();
            x1 = r * Math.cos(t) + 160;
            y1 = r * Math.sin(t) + 120;
            System.out.println("r: " + r + " t: " + t);
            g.drawLine((int) x1, (int) y1, (int) (x1 + 20), (int) (y1 + 20 * Math.tan(t + Math.PI/2))); 
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
		this.magImg = U.biFromLog(out[2]);
		this.edgeImg = U.biFromLog(out[3]);

        System.out.print("image loaded");
        try {
		    this.houghLines = VisionFieldOuterClass.Lines.parseFrom(out[1].bytes);
        } catch (InvalidProtocolBufferException e) {
            System.out.print("proto was not valid\n");
        }
		repaint();
	}
}
